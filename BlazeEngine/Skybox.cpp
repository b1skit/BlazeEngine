#include "Skybox.h"
#include "Material.h"
#include "Mesh.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"
#include "ImageBasedLight.h"
#include "Shader.h"
#include "Texture.h"


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
		this->skyMaterial = new Material("SkyboxMaterial", nullptr, CUBE_MAP_NUM_FACES, false);

		// Attempt to create Skybox from 6x skybox textures:
		//--------------------------------------------------

		// Create/import cube map face textures:
		string skyboxTextureNames[CUBE_MAP_NUM_FACES] =
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

		string skyboxTextureRoot = CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("sceneRoot") + sceneName + "\\Skybox\\";

		// Track the textures as we load them:
		Texture* cubemapTextures[CUBE_MAP_NUM_FACES];
		for (int i = 0; i < CUBE_MAP_NUM_FACES; i++)
		{
			cubemapTextures[i] = nullptr;
		}

		bool foundSkyboxFace = false;
		for (int i = 0; i < CUBE_MAP_NUM_FACES; i++)
		{
			string currentSkyCubeFaceName = skyboxTextureRoot + skyboxTextureNames[i];

			foundSkyboxFace = false;
			for (int j = 0; j < NUM_FILE_EXTENSIONS; j++)
			{
				string finalName = currentSkyCubeFaceName + fileExtensions[j];

				Texture* currentFaceTexture = Texture::LoadTextureFileFromPath(finalName, false, false, false);
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

					currentFaceTexture->TextureUnit()		= CUBE_MAP_0;

					break;
				}
			}

			if (!foundSkyboxFace)
			{
				LOG("Could not find skybox cubemap face texture #" + to_string(i) + ": " + skyboxTextureNames[i] + " with any supported extension. Attempting to load IBL texture instead...");

				// Cleanup:
				for (int i = 0; i < CUBE_MAP_NUM_FACES; i++)
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

		// Create a skybox from an IBL texture, if failing to load a skybox cubemap failed:
		//---------------------------------------------------------------------------------
		if (!foundSkyboxFace)
		{
			Texture** iblAsSkyboxCubemap = (Texture**)ImageBasedLight::ConvertEquirectangularToCubemap(CoreEngine::GetSceneManager()->GetCurrentSceneName(), CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("defaultIBLPath"), 1024, 1024); // TODO: Parameterize cubemap dimensions?

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

		

		// Create a skybox shader, now that we have some sort of image loaded:
		Shader* skyboxShader = Shader::CreateShader(CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("skyboxShaderName"));
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


