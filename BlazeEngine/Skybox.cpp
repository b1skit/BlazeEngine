#include "Skybox.h"
#include "Material.h"
#include "Mesh.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"
#include "ImageBasedLight.h"


namespace BlazeEngine
{
	Skybox::Skybox(Material* skyMaterial, Mesh* skyMesh)
	{
		this->skyMaterial	= skyMaterial;
		this->skyMesh		= skyMesh;
	}


	Skybox::Skybox(string sceneName)
	{
		// Create a cube map material
		this->skyMaterial = new Material("SkyboxMaterial", nullptr, CUBE_MAP_COUNT, false);

		// Create/import cube map face textures:
		string skyboxTextureNames[CUBE_MAP_COUNT] =
		{
			"posx",
			"negx",
			"posy",
			"negy",
			"posz",
			"negz",
		};

		const int NUM_FILE_EXTENSIONS = 4;
		string fileExtensions[NUM_FILE_EXTENSIONS] =	// Add any desired skybox texture filetype extensions here
		{
			".jpg",
			".jpeg",
			".png",
			".tga",
		};

		string skyboxTextureRoot = CoreEngine::GetCoreEngine()->GetConfig()->scene.sceneRoot + sceneName + "\\Skybox\\";

		// Track the textures as we load them:
		Texture* cubemapTextures[CUBE_MAP_COUNT];
		for (int i = 0; i < CUBE_MAP_COUNT; i++)
		{
			cubemapTextures[i] = nullptr;
		}

		bool foundSkyboxFace;
		for (int i = 0; i < CUBE_MAP_COUNT; i++)
		{
			string currentSkyCubeFaceName = skyboxTextureRoot + skyboxTextureNames[i];

			foundSkyboxFace = false;
			for (int j = 0; j < NUM_FILE_EXTENSIONS; j++)
			{
				string finalName = currentSkyCubeFaceName + fileExtensions[j];

				Texture* currentFaceTexture = Texture::LoadTextureFileFromPath(finalName, false, true, false);
				if (currentFaceTexture != nullptr)
				{
					skyMaterial->AccessTexture((TEXTURE_TYPE)i) = currentFaceTexture;
					cubemapTextures[i] = currentFaceTexture;	// Track the face

					foundSkyboxFace = true;

					// Configure the texture:
					currentFaceTexture->TextureTarget()		= GL_TEXTURE_CUBE_MAP;

					currentFaceTexture->TextureWrap_S()		= GL_CLAMP_TO_EDGE;
					currentFaceTexture->TextureWrap_T()		= GL_CLAMP_TO_EDGE;
					currentFaceTexture->TextureWrap_R()		= GL_CLAMP_TO_EDGE;

					currentFaceTexture->TextureMinFilter()	= GL_LINEAR;
					currentFaceTexture->TextureMaxFilter()	= GL_LINEAR;

					currentFaceTexture->InternalFormat()	= GL_SRGB8_ALPHA8;				// Set the diffuse texture's internal format to be encoded in sRGB color space, so OpenGL will apply gamma correction: (ie. color = pow(color, 2.2) )
					// TODO: Should we use this, or use universal shader functions???

					currentFaceTexture->TextureUnit()		= CUBE_MAP_0 + CUBE_MAP_0_RIGHT;	// We always use the same sampler for all cube map faces

					break;
				}
			}

			if (!foundSkyboxFace)
			{
				LOG("Could not find skybox cubemap face texture #" + to_string(i) + ": " + skyboxTextureNames[i] + " with any supported extension. Attempting to load IBL texture instead...");

				// Cleanup:
				for (int i = 0; i < CUBE_MAP_COUNT; i++)
				{
					if (cubemapTextures[i] != nullptr)
					{
						cubemapTextures[i]->Destroy();
						delete cubemapTextures[i];
					}
				}

				break;
			}
		}

		// If we failed to load the skybox cubemap, try and use an IBL texture:
		if (!foundSkyboxFace)
		{
			Texture** iblAsSkyboxCubemap = (Texture**)ImageBasedLight::ConvertEquirectangularToCubemap(CoreEngine::GetSceneManager()->GetCurrentSceneName(), CoreEngine::GetCoreEngine()->GetConfig()->renderer.defaultIBLPath, 1024, 1024); // TODO: Parameterize cubemap dimensions?

			if (iblAsSkyboxCubemap == nullptr)
			{
				this->skyMaterial->Destroy();
				delete skyMaterial;
				this->skyMaterial = nullptr;
				return;
			}

			LOG("Successfully loaded IBL HDR texture for skybox");

			skyMaterial->AttachCubeMapTextures(iblAsSkyboxCubemap);
		}

		// Create a skybox shader:
		Shader* skyboxShader = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->shader.skyboxShaderName);
		skyMaterial->GetShader() = skyboxShader;

		// Configure and buffer textures:
		if (!Texture::BufferCubeMap(&skyMaterial->AccessTexture(TEXTURE_0)))
		{
			LOG_ERROR("Skybox cube map buffering failed");

			skyboxShader->Destroy();
			delete skyboxShader;
			skyboxShader = nullptr;

			skyMaterial->Destroy();
			delete skyMaterial;
			skyMaterial = nullptr;

			return;
		}

		// Create a quad at furthest point in the depth buffer
		this->skyMesh = new Mesh
		(
			Mesh::CreateQuad
			(
				vec3(-1.0f, 1.0f,	1.0f), // z == 1.0f, since we're in clip space (and camera's negative Z has been reversed)
				vec3(1.0f,	1.0f,	1.0f),
				vec3(-1.0f, -1.0f,	1.0f),
				vec3(1.0f,	-1.0f,	1.0f)
			)
		);

		this->skyMesh->Name() = "SkyboxQuad";
	}


	Skybox::~Skybox()
	{
		if (skyMaterial != nullptr)
		{
			delete skyMaterial;
			skyMaterial = nullptr;
		}

		if (skyMesh != nullptr)
		{
			delete skyMesh;
			skyMesh = nullptr;
		}
	}
}


