#include "ImageBasedLight.h"
#include "CoreEngine.h"
#include "Texture.h"
#include "BuildConfiguration.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "RenderTexture.h"

#include "glm.hpp"


namespace BlazeEngine
{
	ImageBasedLight::ImageBasedLight(string lightName, string relativeHDRPath) : Light(lightName, LIGHT_AMBIENT_IBL, vec3(0))
	{
		// IEM setup:
		this->IEM_Material = new Material("IEM_Material", nullptr, CUBE_MAP_NUM_FACES, true);

		Texture** IEM_Textures = (Texture**)ConvertEquirectangularToCubemap(CoreEngine::GetSceneManager()->GetCurrentSceneName(), relativeHDRPath, this->xRes, this->yRes, IBL_IEM);

		if (IEM_Textures != nullptr)
		{
			this->IEM_isValid = true;

			this->IEM_Material->AttachCubeMapTextures(IEM_Textures);
		}

		// PMREM setup:
		this->PMREM_Material = new Material("PMREM_Material", nullptr, CUBE_MAP_NUM_FACES, true);

		Texture** PMREM_Textures = (Texture**)ConvertEquirectangularToCubemap(CoreEngine::GetSceneManager()->GetCurrentSceneName(), relativeHDRPath, this->xRes, this->yRes, IBL_PMREM);

		if (PMREM_Textures != nullptr)
		{
			this->PMREM_isValid = true;

			this->PMREM_Material->AttachCubeMapTextures(PMREM_Textures);

			this->maxMipLevel = (int)glm::log2((float)this->xRes);	// Note: We assume the cubemap is always square and use xRes only during our calculations...
		}

		// Render BRDF Integration map:
		GenerateBRDFIntegrationMap();

		// Upload shader parameters:
		if (this->DeferredMaterial() != nullptr && this->DeferredMaterial()->GetShader() != nullptr)
		{
			this->DeferredMaterial()->GetShader()->UploadUniform("maxMipLevel", &this->maxMipLevel, UNIFORM_Int);
		}
		else
		{
			LOG_ERROR("ImageBasedLight could not upload shader parameters");
		}
	}

	
	ImageBasedLight::~ImageBasedLight()
	{
		if (this->IEM_Material != nullptr)
		{
			this->IEM_Material->Destroy();
			delete this->IEM_Material;
			this->IEM_Material	= nullptr;
			this->IEM_isValid	= false;
		}

		if (this->PMREM_Material != nullptr)
		{
			this->PMREM_Material->Destroy();
			delete this->PMREM_Material;
			this->PMREM_Material	= nullptr;
			this->PMREM_isValid		= false;
		}

		if (this->BRDF_integrationMap != nullptr)
		{
			this->BRDF_integrationMap->Destroy();
			delete this->BRDF_integrationMap;
			this->BRDF_integrationMap = nullptr;
		}
	}


	RenderTexture** ImageBasedLight::ConvertEquirectangularToCubemap(string sceneName, string relativeHDRPath, int xRes, int yRes, IBL_TYPE iblType /*= RAW_HDR*/)
	{
		string cubemapName;
		vector<string> shaderKeywords;
		int textureUnit = -1;	// For now, derrive the cube map texture unit based on the type of texture
		if (iblType == IBL_IEM)
		{
			cubemapName = "IBL_IEM";
			shaderKeywords.push_back("BLIT_IEM");
			textureUnit = (int)CUBE_MAP_0;
		}
		else if (iblType == IBL_PMREM)
		{
			cubemapName = "IBL_PMREM";
			shaderKeywords.push_back("BLIT_PMREM");
			textureUnit = (int)CUBE_MAP_1;
		}
		else
		{
			cubemapName = "HDR_Image";
			textureUnit = (int)CUBE_MAP_0;
			// No need to insert any shader keywords
		}

		// Create our conversion shader:
		string shaderName							= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("equilinearToCubemapBlitShaderName");
		Shader* equirectangularToCubemapBlitShader	= Shader::CreateShader(shaderName, &shaderKeywords);
		if (equirectangularToCubemapBlitShader == nullptr)
		{
			LOG_ERROR("Failed to load equilinearToCubemapBlitShader, cannot convert HDR image to cubemap");
			return nullptr;
		}
		equirectangularToCubemapBlitShader->Bind(true);

		// Load the HDR image:
		string iblTexturePath	= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("sceneRoot") + sceneName + "\\" + relativeHDRPath;
		Texture* hdrTexture		= CoreEngine::GetCoreEngine()->GetSceneManager()->FindLoadTextureByPath(iblTexturePath); // Deallocated by SceneManager

		if (hdrTexture == nullptr)
		{
			LOG_ERROR("Failed to load HDR texture \"" + iblTexturePath + "\" for image-based lighting");

			// Cleanup:
			equirectangularToCubemapBlitShader->Destroy();
			delete equirectangularToCubemapBlitShader;

			return nullptr;
		}

		// Set texture params:
		hdrTexture->TextureWrap_S()		= GL_CLAMP_TO_EDGE;
		hdrTexture->TextureWrap_T()		= GL_CLAMP_TO_EDGE;
		
		hdrTexture->TextureMinFilter()	= GL_LINEAR;
		hdrTexture->TextureMaxFilter()	= GL_LINEAR;					// Default

		hdrTexture->Buffer(TEXTURE_0 + TEXTURE_ALBEDO);

		hdrTexture->Bind(TEXTURE_0 + TEXTURE_ALBEDO, true);

		// Create a cubemap to render the IBL into:
		RenderTexture** cubeFaces		= RenderTexture::CreateCubeMap(xRes, yRes, cubemapName); // Note: RenderTexture constructor caches texture unit supplied here
		
		// Update the cubemap texture parameters:
		for (int i = 0; i < CUBE_MAP_NUM_FACES; i++)
		{
			cubeFaces[i]->Format()			= GL_RGB;
			cubeFaces[i]->InternalFormat()	= GL_RGB16F;

			if (iblType == IBL_PMREM)
			{
				cubeFaces[i]->TextureMinFilter() = GL_LINEAR_MIPMAP_LINEAR;
			}
			else
			{
				cubeFaces[i]->TextureMinFilter() = GL_LINEAR;
			}

			cubeFaces[i]->TextureMaxFilter()	= GL_LINEAR;

			cubeFaces[i]->AttachmentPoint()		= GL_COLOR_ATTACHMENT0 + 0;
			cubeFaces[i]->DrawBuffer()			= GL_COLOR_ATTACHMENT0 + 0;
			cubeFaces[i]->ReadBuffer()			= GL_COLOR_ATTACHMENT0 + 0;
		}

		// Generate mip-maps for PMREM IBL cubemap faces, to ensure they're allocated once we want to write into them:
		if (iblType == IBL_PMREM)
		{
			RenderTexture::BufferCubeMap(cubeFaces, textureUnit);

			cubeFaces[0]->GenerateMipMaps();
		}
		else
		{
			RenderTexture::BufferCubeMap(cubeFaces, textureUnit);
		}

		// Create a cube mesh for rendering:
		Mesh cubeMesh = Mesh::CreateCube();
		cubeMesh.Bind(true);


		// Set shader parameters:
		//-----------------------
		int numSamples;
		if (iblType == IBL_IEM)
		{
			numSamples = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("numIEMSamples");
		}
		else if (iblType == IBL_PMREM)
		{
			numSamples = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("numPMREMSamples");
		}
		equirectangularToCubemapBlitShader->UploadUniform("numSamples", &numSamples, UNIFORM_Int); // "numSamples" is defined directly in equilinearToCubemapBlitShader.frag

		// Upload the texel size for the hdr texture:
		vec4 texelSize = hdrTexture->TexelSize();
		equirectangularToCubemapBlitShader->UploadUniform("texelSize", &texelSize.x, UNIFORM_Vec4fv);

		// Create and upload projection matrix:
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		equirectangularToCubemapBlitShader->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

		// Create view matrices: Orient the camera towards each face of the cube
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};


		// Render into the cube map:
		//--------------------------
		glViewport(0, 0, xRes, yRes);	// Configure viewport to match the cubemap dimensions
		glDepthFunc(GL_LEQUAL);			// Ensure we can render on the far plane
		glDisable(GL_CULL_FACE);		// Disable back-face culling, since we're rendering a cube from the inside

		cubeFaces[0]->BindFramebuffer(true);
		
		// Handle per-mip-map rendering:
		if (iblType == IBL_PMREM)
		{
			// Create a render buffer 
			cubeFaces[0]->CreateRenderbuffer(true);

			// Calculate the number of mip levels we need to render:
			int numMipLevels = (int)glm::log2((float)xRes) + 1;	// Note: We assume the cubemap is always square and use xRes only during our calculations...


			for (int currentMipLevel = 0; currentMipLevel < numMipLevels; currentMipLevel++)
			{
				int mipSize = (int)(xRes / glm::pow(2.0f, currentMipLevel) ); // xRes, xRes/2, xRes/4, ...

				cubeFaces[0]->CreateRenderbuffer(true, mipSize, mipSize);

				glViewport(0, 0, mipSize, mipSize);

				// Compute the roughness for the current mip level, and upload it to the shader:
				float roughness = (float)currentMipLevel / (float)(numMipLevels - 1);
				equirectangularToCubemapBlitShader->UploadUniform("roughness", &roughness, UNIFORM_Float);

				// Render each cube face:
				for (int i = 0; i < CUBE_MAP_NUM_FACES; ++i)
				{
					equirectangularToCubemapBlitShader->UploadUniform("in_view", &captureViews[i][0].x, UNIFORM_Matrix4fv);

					// Attach our cube map face texture as a framebuffer object:
					cubeFaces[i]->AttachToFramebuffer(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, currentMipLevel);

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glDrawElements(GL_TRIANGLES, cubeMesh.NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
				}
			}
		}
		else // Handle non-mip-mapped rendering:
		{
			// Render each cube face:
			for (int i = 0; i < CUBE_MAP_NUM_FACES; ++i)
			{
				equirectangularToCubemapBlitShader->UploadUniform("in_view", &captureViews[i][0].x, UNIFORM_Matrix4fv);

				// Attach our cube map face texture as a framebuffer object:
				cubeFaces[i]->AttachToFramebuffer(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDrawElements(GL_TRIANGLES, cubeMesh.NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
			}
		}
		
		cubeFaces[0]->BindFramebuffer(false);

		// Restore defaults:
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		// Cleanup:
		hdrTexture->Bind(TEXTURE_0 + TEXTURE_ALBEDO, false); // Unbind: Texture will be destroyed/deleted by the SceneManager
		
		cubeMesh.Bind(false);
		cubeMesh.Destroy();

		equirectangularToCubemapBlitShader->Bind(false);
		equirectangularToCubemapBlitShader->Destroy();
		delete equirectangularToCubemapBlitShader;

		cubeFaces[0]->DeleteRenderbuffer(true);

		return cubeFaces;
	}


	void ImageBasedLight::GenerateBRDFIntegrationMap()
	{
		// Destroy any existing map
		if (this->BRDF_integrationMap != nullptr)
		{
			this->BRDF_integrationMap->Destroy();
			delete this->BRDF_integrationMap;
			this->BRDF_integrationMap = nullptr;
		}

		LOG("Rendering BRDF Integration map texture");
		
		// Create a shader:
		string shaderName = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("BRDFIntegrationMapShaderName");
		Shader* BRDFIntegrationMapShader = Shader::CreateShader(shaderName);

		if (BRDFIntegrationMapShader == nullptr)
		{
			LOG_ERROR("Failed to load \"" + shaderName + "\", BRDF Integration map generation failed.");
			return;
		}
		BRDFIntegrationMapShader->Bind(true);

		
		// Create a render texture:
		this->BRDF_integrationMap = new RenderTexture(this->xRes, this->yRes, "BRDFIntegrationMap");

		// Set texture params:
		this->BRDF_integrationMap->TextureWrap_S()		= GL_CLAMP_TO_EDGE;
		this->BRDF_integrationMap->TextureWrap_T()		= GL_CLAMP_TO_EDGE;

		this->BRDF_integrationMap->TextureMinFilter()	= GL_LINEAR;
		this->BRDF_integrationMap->TextureMaxFilter()	= GL_LINEAR;					// Default

		// 2 channel, 16-bit floating point precision, as recommended by Epic Games:
		this->BRDF_integrationMap->InternalFormat()		= GL_RG16F;
		this->BRDF_integrationMap->Format()				= GL_RG;

		this->BRDF_integrationMap->AttachmentPoint()	= GL_COLOR_ATTACHMENT0 + 0;
		this->BRDF_integrationMap->DrawBuffer()			= GL_COLOR_ATTACHMENT0 + 0;

		if (!this->BRDF_integrationMap->Buffer(GENERIC_TEXTURE_0))
		{
			LOG_ERROR("Could not buffer BRDF Integration Map RenderTexture!");
			return;
		}

		this->BRDF_integrationMap->Bind(GENERIC_TEXTURE_0, true);
		
		// Create a CCW screen-aligned quad to render with:
		Mesh quad = Mesh::CreateQuad
		(
			vec3(-1.0f, 1.0f,	-1.0f),	// TL
			vec3(1.0f,	1.0f,	-1.0f),	// TR
			vec3(-1.0f, -1.0f,	-1.0f),	// BL
			vec3(1.0f,	-1.0f,	-1.0f)	// BR
		);
		quad.Bind(true);

		// Render into the quad:
		//--------------------------
		glViewport(0, 0, this->xRes, this->yRes);	// Configure viewport to match the cubemap dimensions
		glDepthFunc(GL_LEQUAL);						// Ensure we can render on the far plane

		this->BRDF_integrationMap->BindFramebuffer(true);
		this->BRDF_integrationMap->CreateRenderbuffer();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, quad.NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);


		// Cleanup:
		quad.Bind(false);

		this->BRDF_integrationMap->BindFramebuffer(false);
		this->BRDF_integrationMap->DeleteRenderbuffer();
		this->BRDF_integrationMap->Bind(GENERIC_TEXTURE_0, false);

		BRDFIntegrationMapShader->Bind(false);
		BRDFIntegrationMapShader->Destroy();
		delete BRDFIntegrationMapShader;		
	}
}