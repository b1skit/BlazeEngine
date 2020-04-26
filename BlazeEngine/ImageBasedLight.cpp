#include "ImageBasedLight.h"
#include "CoreEngine.h"
#include "Texture.h"
#include "BuildConfiguration.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "RenderTexture.h"


namespace BlazeEngine
{
	ImageBasedLight::ImageBasedLight(string lightName, string relativeHDRPath) : Light(lightName, LIGHT_AMBIENT_IBL, vec3(0))
	{
		string matName = "IEM_Material";

		this->iblMaterial = new Material(matName, nullptr, CUBE_MAP_COUNT, true);

		Texture** iblTextures = (Texture**)ConvertEquirectangularToCubemap(CoreEngine::GetSceneManager()->GetCurrentSceneName(), relativeHDRPath, this->xRes, this->yRes, IBL_IEM);

		if (iblTextures != nullptr)
		{
			this->isValid = true;

			this->iblMaterial->AttachCubeMapTextures(iblTextures);
		}

		

		// TODO: Implement a PMREM_Material?
	}

	
	ImageBasedLight::~ImageBasedLight()
	{
		if (iblMaterial != nullptr)
		{
			this->iblMaterial->Destroy();
			delete this->iblMaterial;
			this->iblMaterial = nullptr;
		}
	}


	RenderTexture** ImageBasedLight::ConvertEquirectangularToCubemap(string sceneName, string relativeHDRPath, int xRes, int yRes, IBL_TYPE iblType /*= RAW_HDR*/)
	{
		// TODO: Support loading/processing of PMREM IBL's

		
		string cubemapName;
		vector<string> shaderKeywords;
		if (iblType == IBL_IEM)
		{
			cubemapName = "IBL_IEM";
			shaderKeywords.push_back("BLIT_IEM");
		}
		else if (iblType == IBL_PMREM)
		{
			cubemapName = "IBL_PMREM";
			shaderKeywords.push_back("BLIT_PMREM");
		}
		else
		{
			cubemapName = "HDR_Image";
			// No need to insert any shader keywords
		}

		// Create our conversion shader:
		string shaderName							= "equilinearToCubemapBlitShader";
		Shader* equirectangularToCubemapBlitShader	= Shader::CreateShader(shaderName, &shaderKeywords);
		if (equirectangularToCubemapBlitShader == nullptr)
		{
			LOG_ERROR("Failed to load equilinearToCubemapBlitShader, cannot convert HDR image to cubemap");
			return nullptr;
		}
		equirectangularToCubemapBlitShader->Bind(true);

		// Load the HDR image:
		string iblTexturePath	= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("sceneRoot") + sceneName + "\\" + relativeHDRPath;
		Texture* hdrTexture		= Texture::LoadTextureFileFromPath(iblTexturePath, false, false);

		if (hdrTexture == nullptr)
		{
			LOG_ERROR("Failed to load HDR texture for image-based lighting");

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
		hdrTexture->TextureUnit()		= TEXTURE_0 + TEXTURE_ALBEDO;	// Assign to the "albedo" sampler

		hdrTexture->Buffer();
		hdrTexture->Bind(equirectangularToCubemapBlitShader->ShaderReference());
	
		// Create a cubemap to render the IBL into:
		RenderTexture** cubeFaces	= RenderTexture::CreateCubeMap(xRes, yRes, cubemapName); // Sets texture unit as CUBE_MAP_0 + CUBE_MAP_0_RIGHT
		
		// Update the parameters:
		cubeFaces[0]->TextureTarget()		= GL_TEXTURE_CUBE_MAP;
		cubeFaces[0]->Format()				= GL_RGB;
		cubeFaces[0]->InternalFormat()		= GL_RGB16F;
		cubeFaces[0]->Type()				= GL_FLOAT;

		cubeFaces[0]->TextureWrap_S()		= GL_CLAMP_TO_EDGE;
		cubeFaces[0]->TextureWrap_T()		= GL_CLAMP_TO_EDGE;
		cubeFaces[0]->TextureWrap_R()		= GL_CLAMP_TO_EDGE;

		cubeFaces[0]->TextureMinFilter()	= GL_LINEAR;
		cubeFaces[0]->TextureMaxFilter()	= GL_LINEAR;

		cubeFaces[0]->AttachmentPoint()		= GL_COLOR_ATTACHMENT0 + 0;
		cubeFaces[0]->DrawBuffer()			= GL_COLOR_ATTACHMENT0 + 0;
		cubeFaces[0]->ReadBuffer()			= GL_COLOR_ATTACHMENT0 + 0;

		RenderTexture::BufferCubeMap(cubeFaces);

		// Create a cube mesh for rendering:
		Mesh cubeMesh = Mesh::CreateCube();
		cubeMesh.Bind(true);

		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		equirectangularToCubemapBlitShader->UploadUniform("in_projection", &projection[0][0], UNIFORM_Matrix4fv);

		// "View" matrices: Orient the camera towards each face of the cube
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		int numIEMSamples = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("numIEMSamples");
		equirectangularToCubemapBlitShader->UploadUniform("numSamples", &numIEMSamples, UNIFORM_Int); // "numSamples" is defined directly in the shader

		// Render into the cube map:
		glViewport(0, 0, xRes, yRes);	// Configure viewport to match the cubemap dimensions
		glDepthFunc(GL_LEQUAL);			// Ensure we can render on the far plane
		glDisable(GL_CULL_FACE);		// Disable back-face culling, since we're rendering a cube from the inside

		glBindFramebuffer(GL_FRAMEBUFFER, cubeFaces[0]->FBO());

		for (int i = 0; i < CUBE_MAP_COUNT; ++i)
		{
			equirectangularToCubemapBlitShader->UploadUniform("in_view", &captureViews[i][0].x, UNIFORM_Matrix4fv);

			// Attach our cube map face texture as a framebuffer object:
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeFaces[i]->TextureID(), 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDrawElements(GL_TRIANGLES, cubeMesh.NumIndices(), GL_UNSIGNED_INT, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Restore defaults:
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		// Cleanup:
		hdrTexture->Bind(); // unbind
		hdrTexture->Destroy();
		delete hdrTexture;
		
		cubeMesh.Bind(false);
		cubeMesh.Destroy();

		equirectangularToCubemapBlitShader->Bind(false);
		equirectangularToCubemapBlitShader->Destroy();
		delete equirectangularToCubemapBlitShader;

		return cubeFaces; // TODO: Generate mipmaps for final cubemap?
	}
}