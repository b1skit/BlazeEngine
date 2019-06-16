#include "BuildConfiguration.h"
#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"


#include "glm.hpp"
#include "gtc/constants.hpp"

using glm::pi;


#include "assimp/Importer.hpp"		// Importer interface
#include "assimp/postprocess.h"		// Post processing flags


#define STB_IMAGE_IMPLEMENTATION	// Only include this define ONCE in the project
#include "stb_image.h"				// STB image loader


#include <algorithm>
#include <string>
#include <stdio.h>

#define INVALID_TEXTURE_PATH "InvalidTexturePath"


namespace BlazeEngine
{
	Scene::Scene()
	{
		gameObjects.reserve(GAMEOBJECTS_RESERVATION_AMT);
		renderables.reserve(RENDERABLES_RESERVATION_AMT);
		meshes.reserve(MESHES_RESERVATION_AMT);

		sceneCameras.reserve(CAMERA_TYPE_COUNT);
		for (int i = 0; i < CAMERA_TYPE_COUNT; i++)
		{
			sceneCameras.push_back(vector<Camera*>());
		}
		sceneCameras.at(CAMERA_TYPE_SHADOW).reserve(CAMERA_TYPE_SHADOW_ARRAY_SIZE);
		sceneCameras.at(CAMERA_TYPE_REFLECTION).reserve(CAMERA_TYPE_REFLECTION_ARRAY_SIZE);
		
		sceneCameras.at(CAMERA_TYPE_MAIN).reserve(1); // Only 1 main camera


		/*forwardLights.reserve(100);*/
		/*deferredLights.reserve(100);*/
	}


	Scene::~Scene()
	{
		DeleteMeshes();

		for (int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects.at(i))
			{
				delete gameObjects.at(i);
				gameObjects.at(i) = nullptr;
			}
		}

		ClearCameras();
	}


	void Scene::InitMeshArray()
	{
		DeleteMeshes();
		meshes.reserve(MESHES_RESERVATION_AMT);
	}
	

	int Scene::AddMesh(Mesh* newMesh)
	{		
		// Update scene (world) bounds to contain the new mesh:
		Bounds meshWorldBounds(newMesh->localBounds.GetTransformedBounds(newMesh->GetTransform().Model()));

		if (meshWorldBounds.xMin < sceneWorldBounds.xMin)
		{
			sceneWorldBounds.xMin = meshWorldBounds.xMin;
		}
		if (meshWorldBounds.xMax > sceneWorldBounds.xMax)
		{
			sceneWorldBounds.xMax = meshWorldBounds.xMax;
		}

		if (meshWorldBounds.yMin < sceneWorldBounds.yMin)
		{
			sceneWorldBounds.yMin = meshWorldBounds.yMin;
		}
		if (meshWorldBounds.yMax > sceneWorldBounds.yMax)
		{
			sceneWorldBounds.yMax = meshWorldBounds.yMax;
		}

		if (meshWorldBounds.zMin < sceneWorldBounds.zMin)
		{
			sceneWorldBounds.zMin = meshWorldBounds.zMin;
		}
		if (meshWorldBounds.zMax > sceneWorldBounds.zMax)
		{
			sceneWorldBounds.zMax = meshWorldBounds.zMax;
		}

		// Add the mesh to our array:
		int meshIndex = (int)meshes.size();
		meshes.push_back(newMesh);
		return meshIndex;

	}


	void Scene::DeleteMeshes()
	{
		for (int i = 0; i < (int)meshes.size(); i++)
		{
			if (meshes.at(i) != nullptr)
			{
				delete meshes[i];
				meshes.at(i) = nullptr;
			}
		}
		
		meshes.clear();
	}


	Mesh* Scene::GetMesh(int meshIndex)
	{
		if (meshIndex >= (int)meshes.size())
		{
			LOG_ERROR("Invalid mesh index received: " + to_string(meshIndex) + " > " + to_string((int)meshes.size()) + ". Returning nullptr");
			return nullptr;
		}

		return meshes.at(meshIndex);
	}


	void Scene::RegisterCamera(CAMERA_TYPE cameraType, Camera* newCamera)
	{
		if (newCamera != nullptr && (int)cameraType < (int)sceneCameras.size())
		{
			sceneCameras.at((int)cameraType).push_back(newCamera);

			LOG("Registered new camera \"" + newCamera->GetName() + "\"");
		}
		else
		{
			LOG_ERROR("Failed to register new camera!");
		}
	}


	vector<Camera*> const& Scene::GetCameras(CAMERA_TYPE cameraType)
	{ 
		return sceneCameras.at(cameraType);
	}


	void Scene::ClearCameras()
	{
		if (sceneCameras.empty())
		{
			return;
		}

		for (int i = 0; i < (int)sceneCameras.size(); i++)
		{
			for (int j = 0; j < (int)sceneCameras.at(i).size(); j++)
			{
				if (sceneCameras.at(i).at(j) != nullptr)
				{
					sceneCameras.at(i).at(j)->Destroy();
					delete sceneCameras.at(i).at(j);
					sceneCameras.at(i).at(j) = nullptr;
				}
			}
		}
	}


	/***************************************************************************************************************************************
	SCENE MANAGER
	****************************************************************************************************************************************/

	SceneManager::SceneManager() : EngineComponent("SceneManager")
	{
		stbi_set_flip_vertically_on_load(true);	// Set stb_image to flip the y-axis on loading to match OpenGL's style (So pixel (0,0) is in the bottom-left of the image)
	}


	SceneManager& SceneManager::Instance()
	{
		static SceneManager* instance = new SceneManager();
		return *instance;
	}


	void SceneManager::Startup()
	{
		// Initialize materials:
		materials = new Material*[MAX_MATERIALS];
		for (unsigned int i = 0; i < MAX_MATERIALS; i++)
		{
			materials[i] = nullptr;
		}
		currentMaterialCount = 0;

		textures = new Texture*[MAX_TEXTURES];
		for (unsigned int i = 0; i < MAX_TEXTURES; i++)
		{
			textures[i] = nullptr;
		}
		currentTextureCount = 0;

		LOG("Scene manager started!");
	}


	void SceneManager::Shutdown()
	{
		LOG("Scene manager shutting down...");

		// Scene cleanup:
		currentScene->keyLight.Destroy();

		if (currentScene)
		{
			delete currentScene;
			currentScene = nullptr;
		}
		

		// Scene manager cleanup:
		if (materials)
		{
			for (unsigned int i = 0; i < MAX_MATERIALS; i++)
			{
				if (materials[i])
				{
					if (materials[i]->GetShader() != nullptr)
					{
						glDeleteProgram(materials[i]->GetShader()->ShaderReference());
						delete materials[i]->GetShader();
					}
					delete materials[i];
					materials[i] = nullptr;
				}
			}
			delete[] materials;
			currentMaterialCount = 0;
		}

		if (textures)
		{
			for (unsigned int i = 0; i < currentTextureCount; i++)
			{
				if (textures[i])
				{
					textures[i]->Destroy();
					delete textures[i];
					textures[i] = nullptr;
				}
			}
			delete[] textures;
			textures = nullptr;
		}
	}


	void SceneManager::Update()
	{
		for (int i = 0; i < (int)currentScene->gameObjects.size(); i++)
		{
			currentScene->gameObjects.at(i)->Update();
		}
	}


	void SceneManager::HandleEvent(EventInfo const * eventInfo)
	{
		return;
	}


	void SceneManager::LoadScene(string sceneName)
	{
		if (sceneName == "")
		{
			LOG_ERROR("Quitting! No scene name received. Did you forget to use the \"-scene theSceneName\" command line argument?");
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, nullptr });
			return;
		}

		if (currentScene)
		{
			LOG("Unloading existing scene");
			delete currentScene;
			currentScene = nullptr;
		}
		currentScene = new Scene();

		// Assemble paths:
		string sceneRoot = CoreEngine::GetCoreEngine()->GetConfig()->scene.sceneRoot + sceneName + "\\";
		string fbxPath = sceneRoot + sceneName + ".fbx";

		// Load our .fbx using Assimp:
		Assimp::Importer importer;
		aiScene const* scene = importer.ReadFile(fbxPath, 
			aiProcess_ValidateDataStructure 
			| aiProcess_CalcTangentSpace
			| aiProcess_Triangulate 
			| aiProcess_JoinIdenticalVertices 
			| aiProcess_SortByPType 
			| aiProcess_GenUVCoords 
			| aiProcess_TransformUVCoords
		); // | aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials

		if (!scene)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, nullptr, new string("Failed to load scene file: " + fbxPath + ": " + importer.GetErrorString() ) });
			return;
		}
		else
		{
			LOG("Successfully loaded scene file " + fbxPath);
		}


		// Extract materials and textures:
		//--------------------------------
		if (scene->HasTextures()) // Check for embedded textures
		{
			int numTextures = scene->mNumTextures;
			LOG_ERROR("Found " + to_string(numTextures) + " embedded scene textures. These will NOT be loaded!");
		}
		#if defined(DEBUG_SCENEMANAGER_MATERIAL_LOGGING)
			else
			{
				LOG("Scene has no embedded textures");
			}
		#endif

		if (scene->HasMaterials())
		{
			ImportMaterialsAndTexturesFromScene(scene, sceneName);
		}
		else
		{
			LOG_ERROR("Scene has no materials");
		}


		// Extract meshes:
		//----------------
		if (scene->HasMeshes())
		{
			ImportGameObjectGeometryFromScene(scene);
		}
		else
		{
			LOG_ERROR("Scene has no meshes");
		}


		// Assemble material mesh lists:
		// -----------------------------
		AssembleMaterialMeshLists();


		// Extract lights:
		//----------------
		if (scene->HasLights())
		{
			ImportLightsFromScene(scene);
		}
		else
		{
			LOG_ERROR("Scene has no lights");
		}


		// Extract cameras:
		//-----------------
		if (scene->HasCameras())
		{
			ImportCamerasFromScene(scene);
		}
		else
		{
			LOG_ERROR("Scene has no camera");
			ImportCamerasFromScene();
		}
			   

		// Create a PlayerObject:
		//-----------------------
		PlayerObject* player = new PlayerObject(currentScene->GetMainCamera());
		currentScene->gameObjects.push_back(player);	
		LOG("Created PlayerObject using mainCamera");
	}


	Material* BlazeEngine::SceneManager::GetMaterial(string materialName)
	{
		for (unsigned int i = 0; i < currentMaterialCount; i++)
		{
			if (materials[i]->Name() == materialName)
			{
				return materials[i];
			}
		}

		LOG_ERROR("Could not find material \"" + materialName + "\"");
		return nullptr;
	}

	vector<Mesh*> const* BlazeEngine::SceneManager::GetRenderMeshes(int materialIndex /*= -1*/)
	{
		// If materialIndex is out of bounds, return ALL meshes
		if (materialIndex < 0 || materialIndex >= (int)materialMeshLists.size())
		{
			return &currentScene->GetMeshes();
		}

		return &materialMeshLists.at(materialIndex);
	}
	
	
	void BlazeEngine::SceneManager::AddGameObject(GameObject* newGameObject)
	{
		currentScene->gameObjects.push_back(newGameObject);

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		currentScene->renderables.push_back(newGameObject->GetRenderable());

		#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
			LOG("Added new GameObject to the scene: " + newGameObject->GetName());
		#endif	
	}


	void BlazeEngine::SceneManager::InitializeTransformValues(aiMatrix4x4 const& source, Transform* dest)
	{
		aiVector3D sourceScale, sourcePosition; // sourceRotation, 
		aiQuaternion sourceRotation;
		source.Decompose(sourceScale, sourceRotation, sourcePosition); // Decompose the source matrix into its scale, rotation, position components

		#if defined(DEBUG_TRANSFORMS)
			LOG("Decomposed aiMatrix4x4 into:");
			LOG("\tSource Scale:    " + to_string(sourceScale.x) + " " + to_string(sourceScale.y) + " " + to_string(sourceScale.z));
			LOG("\tSource Position: " + to_string(sourcePosition.x) + " " + to_string(sourcePosition.y) + " " + to_string(sourcePosition.z));
			LOG("\tSource Rotation: " + to_string(sourceRotation.x) + " " + to_string(sourceRotation.y) + " " + to_string(sourceRotation.z) + " " + to_string(sourceRotation.w));
		#endif

		glm::quat sourceRotationAsGLMQuat(sourceRotation.w, sourceRotation.x, sourceRotation.y, sourceRotation.z);
		vec3 eulerRotation = glm::eulerAngles(sourceRotationAsGLMQuat);

		dest->SetPosition(vec3(sourcePosition.x, sourcePosition.y, sourcePosition.z));
		dest->SetRotation(sourceRotationAsGLMQuat);
		dest->SetScale(vec3(sourceScale.x, sourceScale.y, sourceScale.z));
	}


	int SceneManager::GetMaterialIndex(string materialName)
	{
		// Check if a material with the same name exists, and return it if it does:
		int materialIndex = GetMaterialIndexIfExists(materialName);
		if (materialIndex != -1)
		{
			return materialIndex;
		}
		
		LOG_ERROR("Material \"" + materialName + "\" does not exist... Creating a new material with default shader");

		// If we've made it this far, no material with the given name exists. Create it:
		Material* newMaterial = new Material(materialName, CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName); // Assign the default shader

		return AddMaterial(newMaterial, false);
	}


	unsigned int SceneManager::AddMaterial(Material* newMaterial, bool checkForExisting) // checkForExisting = true by default
	{
		if (checkForExisting) // Check if a material with the same name exists, and return it if it does
		{
			int materialIndex = GetMaterialIndexIfExists(newMaterial->Name());
			if (materialIndex != -1)
			{
				LOG("Found existing material \"" + materials[materialIndex]->Name() + "\" at index " + to_string(materialIndex));
				return materialIndex;
			}
		}			

		// Otherwise, add a new material:
		if (currentMaterialCount == MAX_MATERIALS)
		{
			LOG_ERROR("Cannot add any new materials: Max materials have been added! Returning material at index 0");
			return 0; // Error: Return first material
		}

		materials[currentMaterialCount] = newMaterial;
		unsigned int newIndex = currentMaterialCount;
		currentMaterialCount++;

		return newIndex; // Return the previous index
	}


	int SceneManager::GetMaterialIndexIfExists(string materialName)
	{
		for (unsigned int i = 0; i < MAX_MATERIALS; i++)
		{
			if (materials[i] && materials[i]->Name() == materialName)
			{
				return i;
			}
		}
		return -1;
	}


	void SceneManager::AssembleMaterialMeshLists()
	{
		// Pre-allocate our vector of vectors:
		materialMeshLists.clear();
		materialMeshLists.reserve(currentMaterialCount);
		for (unsigned int i = 0; i < currentMaterialCount; i++)
		{
			materialMeshLists.emplace_back(vector<Mesh*>());
			materialMeshLists.at(i).reserve(25);			// TODO: Tune this value based on the actual number of meshes loaded?
		}

		unsigned int numMeshes = 0;
		for (int i = 0; i < (int)currentScene->renderables.size(); i++)
		{
			for (int j = 0; j < (int)currentScene->renderables.at(i)->ViewMeshes()->size(); j++)
			{
				Mesh* viewMesh = currentScene->renderables.at(i)->ViewMeshes()->at(j);
				int materialIndex = viewMesh->MaterialIndex();
				if (materialIndex < 0 || (unsigned int)materialIndex >= currentMaterialCount)
				{
					LOG("AssembleMaterialMeshLists() is skipping a mesh with out of localBounds material index!");
				}
				else
				{
					materialMeshLists.at(materialIndex).emplace_back(viewMesh);
					numMeshes++;
				}
			}
		}

		LOG("Assembled material mesh list of " + to_string(numMeshes) + " meshes and " + to_string(materialMeshLists.size()) + " materials");
	}


	Texture* BlazeEngine::SceneManager::FindLoadTextureByPath(string texturePath)
	{
		for (unsigned int i = 0; i < currentTextureCount; i++)
		{
			if (textures[i] && textures[i]->TexturePath() == texturePath)
			{
				LOG("Texture at path " + texturePath + " has already been loaded");
				return textures[i];
			}
		}

		// If we've made it this far, load the texture
		currentTextureCount++;
		return Texture::LoadTextureFromPath(texturePath);
	}


	void SceneManager::ImportMaterialsAndTexturesFromScene(aiScene const* scene, string sceneName)
	{
		int numMaterials = scene->mNumMaterials;
		LOG("Found " + to_string(numMaterials) + " scene materials");

		// Create Blaze Engine materials:
		for (int currentMaterial = 0; currentMaterial < numMaterials; currentMaterial++)
		{
			// Get the material name:
			aiString name;
			if (AI_SUCCESS == scene->mMaterials[currentMaterial]->Get(AI_MATKEY_NAME, name))
			{
				string matName = string(name.C_Str());
				LOG("Loading scene material \"" + matName + "\"...");

				#if defined(DEBUG_SCENEMANAGER_MATERIAL_LOGGING)
					LOG("Printing received material property keys:");
					aiMaterial* currentAiMaterial = scene->mMaterials[currentMaterial];
					for (unsigned int i = 0; i < currentAiMaterial->mNumProperties; i++)
					{
						LOG("KEY: " + string(currentAiMaterial->mProperties[i]->mKey.C_Str()));
					}
				#endif

				string rawName = matName; // Convert to lowercase for comparisons
				for (int currentChar = 0; currentChar < rawName.length(); currentChar++)
				{
					if (isalpha(rawName.at(currentChar)) &&  isupper(rawName.at(currentChar)))
					{
						rawName.at(currentChar) = tolower(rawName.at(currentChar));
					}
				}

				Material* newMaterial = nullptr;

				// Assign shader based on the material name
				std::size_t shaderNameIndex = matName.find_last_of("_");
				if (shaderNameIndex == string::npos)
				{
					LOG_ERROR("Could not find a shader name prefixed with an underscore in the material name. Assigning the error shader");

					newMaterial = new Material(matName, CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName);
				}
				else
				{
					string shaderName = matName.substr(shaderNameIndex + 1, matName.length() - (shaderNameIndex + 1));

					#if defined(DEBUG_SCENEMANAGER_MATERIAL_LOGGING)
						LOG("Attempting to assign shader \"" + shaderName + "\" to material");
					#endif

					newMaterial = new Material(matName, shaderName);
				}

				// Extract material properties, and set the in the shader:
				Shader* currentShader = newMaterial->GetShader();
				if (currentShader->Name() != CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName)
				{
					/* NOTE: For simplicity, BlazeEngine interprets phong shaders loaded from FBX files:
					Shader name:	Attempt to use whatever follows the last _underscore as a shader name (Eg. myMaterial_phong)
					Albedo:			Phong's color (rgb)
					Transparency:	Phong's color (a)
					Normal:			Phong's bump (rgb)
					Emissive:		Phong's incandescence (rgb)

					Packed Roughness + Metalic + AO channels:
						Roughness:		Phong's specular color (r)
						Metalic:		Phong's specular color (g)
						AO:				Phong's specular color (b)
					*/

					// Extract material's textures:
					LOG("Importing albedo + transparency texture (RGB+A) from material's diffuse/color slot");
					Texture* diffuseTexture = ExtractLoadTextureFromAiMaterial(aiTextureType_DIFFUSE, scene->mMaterials[currentMaterial], sceneName);
					if (diffuseTexture)
					{
						newMaterial->SetTexture(diffuseTexture, TEXTURE_ALBEDO);
						diffuseTexture->Buffer();
					}

					LOG("Importing normal map texture (RGB) from material's bump slot");
					Texture* normalTexture = ExtractLoadTextureFromAiMaterial(aiTextureType_NORMALS, scene->mMaterials[currentMaterial], sceneName);
					if (normalTexture)
					{
						newMaterial->SetTexture(normalTexture, TEXTURE_NORMAL);
						normalTexture->Buffer();
					}
					else
					{
						newMaterial->AddShaderKeyword("NO_NORMAL_TEXTURE");
					}
				

					LOG("Importing emissive map texture (RGB) from material's incandescence slot");
					Texture* emissiveTexture = ExtractLoadTextureFromAiMaterial(aiTextureType_EMISSIVE, scene->mMaterials[currentMaterial], sceneName);
					if (emissiveTexture)
					{
						newMaterial->SetTexture(emissiveTexture, TEXTURE_EMISSIVE);
						emissiveTexture->Buffer();
					}

					LOG("Importing roughness, metalic, & AO textures (R+G+B) from material's specular slot");
					Texture* RMAO = ExtractLoadTextureFromAiMaterial(aiTextureType_SPECULAR, scene->mMaterials[currentMaterial], sceneName);
					if (RMAO)
					{
						newMaterial->SetTexture(RMAO, TEXTURE_RMAO);
						RMAO->Buffer();
					}

					LOG("Importing value from material's cosine power slot");
					if (ExtractPropertyFromAiMaterial(scene->mMaterials[currentMaterial], newMaterial->Property(MATERIAL_PROPERTY_0), AI_MATKEY_SHININESS))
					{
						#if defined(DEBUG_SCENEMANAGER_SHADER_LOGGING)
							LOG("Setting shader matProperty0 uniform: " + to_string(newMaterial->Property(MATERIAL_PROPERTY_0).x) + ", " + to_string(newMaterial->Property(MATERIAL_PROPERTY_0).y) + ", " + to_string(newMaterial->Property(MATERIAL_PROPERTY_0).z));
						#endif

						currentShader->UploadUniform("matProperty0", &newMaterial->Property(MATERIAL_PROPERTY_0).x, UNIFORM_Vec3fv);
					}
					#if defined(DEBUG_SCENEMANAGER_SHADER_LOGGING)
					else
					{
						LOG_WARNING("Could not find material cosine power slot");
					}
					#endif

					// TODO: Extract more generic properties?
				} 
				#if defined(DEBUG_SCENEMANAGER_SHADER_LOGGING) || defined(DEBUG_SCENEMANAGER_MATERIAL_LOGGING)
					else
					{
						LOG("Shader is the error shader: Skipping attempt to upload any uniform values");
					}
				#endif

				// Add the material to our material list:
				AddMaterial(newMaterial, false);
			}
		}

		LOG("Loaded a total of " + to_string(currentTextureCount) + " textures (including error textures)");
	}

	Texture* BlazeEngine::SceneManager::ExtractLoadTextureFromAiMaterial(aiTextureType textureType, aiMaterial* material, string sceneName)
	{
		Texture* newTexture = nullptr;

		int textureCount = material->GetTextureCount(textureType);
		if (textureCount <= 0)
		{
			// Handle special case texture fallbacks:
			if (textureType == aiTextureType_DIFFUSE)
			{
				aiColor4D color;
				if (AI_SUCCESS == material->Get("$clr.diffuse", 0, 0, color))
				{
					string matTextureName = string(material->GetName().C_Str()) + "_DiffuseColor";

					LOG_WARNING("Material has no diffuse texture. Creating a 1x1 texture using the diffuse color with a path " + matTextureName);
					
					newTexture = new Texture(1, 1, matTextureName, true, vec4(color.r, color.g, color.b, color.a));
				}
			}
			else if (textureType == aiTextureType_NORMALS)
			{
				newTexture = FindTextureByNameInAiMaterial("normal", material, sceneName); // Try and find any likely texture in the material
				
				if (newTexture == nullptr)
				{
					string flatNormalName = "DefaultFlatNormal"; // Use a generic name, so this texture will be shared

					LOG_WARNING("Material has no normal texture. Creating a 1x1 texture for a [0,0,1] normal with a path " + flatNormalName);
					// TODO: Replace this with shader multi-compiles. If no normal texture is found, use vertex normals instead

					newTexture = new Texture(1, 1, flatNormalName, true, vec4(0.5f, 0.5f, 1.0f, 1.0f));
				}				
			}
			else if (aiTextureType_EMISSIVE)
			{
				newTexture = FindTextureByNameInAiMaterial("emissive", material, sceneName);
			}
			else if (textureType == aiTextureType_SPECULAR)
			{
				newTexture = FindTextureByNameInAiMaterial("roughness", material, sceneName);
				if (newTexture == nullptr)
				{
					newTexture = FindTextureByNameInAiMaterial("metallic", material, sceneName);
					if (newTexture == nullptr)
					{
						newTexture = FindTextureByNameInAiMaterial("rmao", material, sceneName);
					}
				}
			}
			else
			{
				LOG_WARNING("Received material does not have the requested texture. Returning nullptr!");
				return nullptr;
			}
			
			return newTexture;
		}

		if (textureCount > 1)
		{
			LOG_WARNING("Received material has " + to_string(textureCount) + " of the requested texture type... Only the first will be extracted");
		}

		string sceneRoot = CoreEngine::GetCoreEngine()->GetConfig()->scene.sceneRoot + sceneName + "\\";

		aiString path;
		material->GetTexture(textureType, 0, &path); // We only get the texture at index 0 (any others are ignored...)
		if (path.length > 0)
		{
			string texturePath = sceneRoot + string(path.C_Str());

			#if defined(DEBUG_SCENEMANAGER_TEXTURE_LOGGING)
				LOG("Found texture path: " + texturePath);
			#endif

			// Find the texture if it has already been loaded, or load it otherwise:
			newTexture = FindLoadTextureByPath(texturePath);
		}
		else
		{
			LOG_ERROR("Material does not contain a diffuse texture path. Assigning an error texture");
		}

		if (newTexture == nullptr)
		{
			newTexture = FindLoadTextureByPath(INVALID_TEXTURE_PATH);
		}
		
		return newTexture;
	}

	Texture* SceneManager::FindTextureByNameInAiMaterial(string nameSubstring, aiMaterial* material, string sceneName)
	{
		std::transform(nameSubstring.begin(), nameSubstring.end(), nameSubstring.begin(), ::tolower);

		for (int currentTextureType = 0; currentTextureType < AI_TEXTURE_TYPE_MAX; currentTextureType++)
		{
			aiString path;
			material->GetTexture((aiTextureType)currentTextureType, 0, &path);
			if (path.length > 0)
			{
				string pathString(path.C_Str());
				std::transform(pathString.begin(), pathString.end(), pathString.begin(), ::tolower);

				if (pathString.find(nameSubstring) != string::npos)
				{
					//pathString = string(path.C_Str());
					LOG_WARNING("Texture not found in expected slot. Assigning texture containing \"" + nameSubstring + "\" as a fallback");

					string sceneRoot = CoreEngine::GetCoreEngine()->GetConfig()->scene.sceneRoot + sceneName + "\\";
					string texturePath = sceneRoot + string(path.C_Str());
					
					return FindLoadTextureByPath(texturePath);					
				}
			}
		}

		return nullptr;
	}

	bool BlazeEngine::SceneManager::ExtractPropertyFromAiMaterial(aiMaterial* material, vec3& targetProperty, char const* AI_MATKEY_TYPE, int unused0 /*= 0*/, int unused1 /*= 0*/)
	{
		aiColor3D color(0.f, 0.f, 0.f);
		if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, color))
		{
			#if defined(DEBUG_SCENEMANAGER_MATERIAL_LOGGING)
				LOG("Successfully extracted material property from AI_MATKEY_SHININESS");
			#endif

			targetProperty = vec3(color.r, color.g, color.b);

			return true;
		}
		else if (AI_SUCCESS == material->Get("$raw.Shininess", 0, 0, color))
		{
			#if defined(DEBUG_SCENEMANAGER_MATERIAL_LOGGING)
				LOG("Successfully extracted material property from $raw.Shininess");
			#endif

			targetProperty = vec3(color.r, color.g, color.b);
			return true;
		}
		else
		{
			#if defined(DEBUG_SCENEMANAGER_MATERIAL_LOGGING)
				LOG_ERROR("Material property extraction failed");
			#endif

			return false;
		}
	}


	void SceneManager::ImportGameObjectGeometryFromScene(aiScene const* scene)
	{
		int numMeshes = scene->mNumMeshes;
		LOG("Found " + to_string(numMeshes) + " scene meshes");

		// Allocations:
		currentScene->InitMeshArray();
		
		currentScene->gameObjects.clear();
		currentScene->gameObjects.reserve(numMeshes); // Assuming that every GameObject will have at least 1 mesh...

		// Loop through each mesh in the scene graph:
		for (int currentMesh = 0; currentMesh < numMeshes; currentMesh++)
		{
			string meshName = string(scene->mMeshes[currentMesh]->mName.C_Str());

			// Check mesh is valid:
			if (
				scene->mMeshes[currentMesh]->HasPositions()
				&& scene->mMeshes[currentMesh]->HasFaces()
				&& scene->mMeshes[currentMesh]->HasNormals()
				&& scene->mMeshes[currentMesh]->HasVertexColors(0)
				&& scene->mMeshes[currentMesh]->HasTextureCoords(0)
				&& scene->mMeshes[currentMesh]->HasTangentsAndBitangents()
				)
			{
				LOG("All expected mesh properties found");
			}
			else
			{
				LOG_WARNING("Mesh \"" + meshName + "\" is missing the following properties:");
				if (!scene->mMeshes[currentMesh]->HasPositions())				LOG_WARNING("\t - positions");
				if (!scene->mMeshes[currentMesh]->HasFaces())					LOG_WARNING("\t - faces");
				if (!scene->mMeshes[currentMesh]->HasNormals())					LOG_WARNING("\t - normals");
				if (!scene->mMeshes[currentMesh]->HasVertexColors(0))			LOG_WARNING("\t - vertex colors");
				if (!scene->mMeshes[currentMesh]->HasTextureCoords(0))			LOG_WARNING("\t - texture coordinates");
				if (!scene->mMeshes[currentMesh]->HasTangentsAndBitangents())	LOG_WARNING("\t - tangents & bitangents");
			}

			// Find the corresponding node in the scene graph:
			aiNode* currentNode = scene->mRootNode->FindNode(scene->mMeshes[currentMesh]->mName);
			if (currentNode)
			{
				// We've found a corresponding node in the scene graph. Create a mesh:
				int numVerts		= scene->mMeshes[currentMesh]->mNumVertices;
				int numFaces		= scene->mMeshes[currentMesh]->mNumFaces;
				int numUVs			= scene->mMeshes[currentMesh]->mNumUVComponents[0]; // Just look at the first UV channel for now...
				int numUVChannels	= scene->mMeshes[currentMesh]->GetNumUVChannels();
				int materialIndex	= scene->mMeshes[currentMesh]->mMaterialIndex;

				#if defined(DEBUG_SCENEMANAGER_MESH_LOGGING)
					LOG("\nMesh #" + to_string(currentMesh) + " \"" + meshName + "\": " + to_string(numVerts) + " verts, " + to_string(numFaces) + " faces, " + to_string(numUVChannels) + " UV channels, " + to_string(numUVs) + " UV components in channel 0, using material #" + to_string(materialIndex));
				#endif

				Vertex* vertices = new Vertex[numVerts];

				// Add each vertex to the vertices array:
				for (int currentVert = 0; currentVert < numVerts; currentVert++)
				{
					// Default vertex values:
					vec3 position = vec3(0,0,0), normal = vec3(0, 0, 0), tangent = vec3(0, 0, 0), bitangent = vec3(0, 0, 0);
					vec4 color(0, 0, 0, 1);
					vec4 uv(0, 0, 0, 0);

					bool hasTangentsAndBitangents = false, hasNormal = false;

					// Position:
					if (scene->mMeshes[currentMesh]->HasPositions())
					{
						position = vec3(scene->mMeshes[currentMesh]->mVertices[currentVert].x, scene->mMeshes[currentMesh]->mVertices[currentVert].y, scene->mMeshes[currentMesh]->mVertices[currentVert].z);
					}

					// Normal:
					if (scene->mMeshes[currentMesh]->HasFaces())
					{
						hasNormal = true;
						normal = vec3(scene->mMeshes[currentMesh]->mNormals[currentVert].x, scene->mMeshes[currentMesh]->mNormals[currentVert].y, scene->mMeshes[currentMesh]->mNormals[currentVert].z);
					}

					// Vertex color:
					if (scene->mMeshes[currentMesh]->HasVertexColors(0) && scene->mMeshes[currentMesh]->mColors[0])
					{
						color = vec4(scene->mMeshes[currentMesh]->mColors[0][currentVert].r, scene->mMeshes[currentMesh]->mColors[0][currentVert].g, scene->mMeshes[currentMesh]->mColors[0][currentVert].b, scene->mMeshes[currentMesh]->mColors[0][currentVert].a);
					}

					// TexCoords:
					if (scene->mMeshes[currentMesh]->HasTextureCoords(0))
					{
						uv = vec4(scene->mMeshes[currentMesh]->mTextureCoords[0][currentVert].x, scene->mMeshes[currentMesh]->mTextureCoords[0][currentVert].y, 0, 0);
					}

					// Tangents/Bitangents:
					if (scene->mMeshes[currentMesh]->HasTangentsAndBitangents())
					{
						hasTangentsAndBitangents = true;
						tangent = vec3(scene->mMeshes[currentMesh]->mTangents[currentVert].x, scene->mMeshes[currentMesh]->mTangents[currentVert].y, scene->mMeshes[currentMesh]->mTangents[currentVert].z);
						bitangent = vec3(scene->mMeshes[currentMesh]->mBitangents[currentVert].x, scene->mMeshes[currentMesh]->mBitangents[currentVert].y, scene->mMeshes[currentMesh]->mBitangents[currentVert].z);
					}

					// Handle incorrect tangents/bitangents due to flipped UV's:
					if (hasNormal && hasTangentsAndBitangents)
					{
						if (glm::dot(glm::cross(tangent, bitangent), normal) < 0)
						{
							tangent *= -1;
						}
					}

					// Assemble the vertex:
					vertices[currentVert] = Vertex(position, normal, tangent, bitangent, color, uv);
				}

				// Fill the indices array:
				int numIndices = scene->mMeshes[currentMesh]->mNumFaces * 3;
				GLuint* indices = new GLuint[numIndices];

				#if defined(DEBUG_SCENEMANAGER_MESH_LOGGING)
					LOG("Created arrays of " + to_string(numVerts) + " vertices, & " + to_string(numIndices) + " indices");
				#endif					

				for (int currentFace = 0; currentFace < numFaces; currentFace++)
				{
					for (int currentIndex = 0; currentIndex < 3; currentIndex++)
					{
						if (scene->mMeshes[currentMesh]->mFaces[currentFace].mNumIndices != 3)
						{
							LOG_ERROR("Found a face that doesn't have 3 indices during mesh import!")
						}
						indices[(currentFace * 3) + currentIndex] = scene->mMeshes[currentMesh]->mFaces[currentFace].mIndices[currentIndex];
					}
				}

				Mesh* newMesh = new Mesh(meshName, vertices, numVerts, indices, numIndices, materialIndex);

				GameObject* gameObject = FindCreateGameObjectParents(scene, currentNode->mParent);

				aiMatrix4x4 combinedTransform;
				Transform* targetTransform = nullptr;

				// If the mesh doesn't belong to a group, create a GameObject to contain it:
				if (gameObject == nullptr)
				{
					#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
						LOG_ERROR("Creating a GameObject for mesh \"" + meshName + "\" that did not belong to a group! GameObjects should belong to groups in the source .FBX!");
					#endif
					
					gameObject = new GameObject(meshName);
					AddGameObject(gameObject);				// Add the new game object

					newMesh->Name() = meshName + "_MESH";	// Add a postfix to remind us that we expect GameObjects to be grouped in our .FBX from Maya

					targetTransform = gameObject->GetTransform(); // We'll use the gameobject in our transform heirarchy
				}
				else // We have a GameObject:
				{
					#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
						LOG("Found existing parent GameObject \"" + gameObject->GetName() + "\" for mesh \"" + meshName + "\"");
					#endif

					targetTransform = &newMesh->GetTransform();	// We'll use the mesh in our transform heirarchy
				}

				combinedTransform = GetCombinedTransformFromHierarchy(scene, currentNode->mParent);	// Mesh doesn't belong to a group, so we'll give it's transform to the gameobject we've created
				combinedTransform = combinedTransform * currentNode->mTransformation;				// Combine the parent and child transforms	
				
				InitializeTransformValues(combinedTransform, targetTransform);						// Copy to our Mesh transform

				gameObject->GetRenderable()->AddViewMeshAsChild(newMesh);							// Creates transform heirarchy

				currentScene->AddMesh(newMesh);														// Also calculates scene bounds
			}
			else
			{
				LOG_ERROR("Could not find \"" + meshName + "\" in the scene graph");
			}
		}

		int numGameObjects = (int)currentScene->gameObjects.size();
		LOG("Created " + to_string(numGameObjects) + " game objects");
	}


	GameObject* BlazeEngine::SceneManager::FindCreateGameObjectParents(aiScene const* scene, aiNode* parent)
	{
		if (parent == nullptr || parent == scene->mRootNode)
		{
			#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
				LOG("Reached end of parent heirarchy!");
			#endif

			return nullptr;
		}

		string parentName = string(parent->mName.C_Str());

		// Exclude Maya .fbx "frozen" transformation nodes and keep searching:
		if (parentName.find("$AssimpFbx$") != string::npos)
		{	
			#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
				LOG("Found Maya transformation node \"" + parentName + "\", ignoring and continuing to search!");
			#endif
			return FindCreateGameObjectParents(scene, parent->mParent);
		}

		// Check if there is a GameObject that corresponds with the current parent node
		for (int i = 0; i < (int)currentScene->gameObjects.size(); i++)
		{
			if (currentScene->gameObjects.at(i)->GetName() == parentName)
			{
				#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
					LOG("Found an existing GameObject parent: \"" + parentName + "\"");
				#endif

				return currentScene->gameObjects.at(i);
			}
		}

		// Otherwise, create the heirarchy
		GameObject* newGameObject = new GameObject(parentName);
		InitializeTransformValues(parent->mTransformation, newGameObject->GetTransform());

		AddGameObject(newGameObject);

		GameObject* nextParent = FindCreateGameObjectParents(scene, parent->mParent);
		if (nextParent)
		{
			#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
				LOG("Parented \"" + newGameObject->GetName() + "\" -> \"" + nextParent->GetName() + "\"");
			#endif

			newGameObject->GetTransform()->Parent(nextParent->GetTransform());
		}
		
		#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
			LOG("Returning newly created GameObject \"" + newGameObject->GetName() + "\"");
		#endif

		return newGameObject;
	}


	aiMatrix4x4 BlazeEngine::SceneManager::GetCombinedTransformFromHierarchy(aiScene const* scene, aiNode* parent, bool skipPostRotations /*= true*/)
	{
		if (scene == nullptr || parent == nullptr)
		{
			LOG_ERROR("SceneManager.GetCombinedTransformFromHierarchy() received a null pointer!");
			return aiMatrix4x4();
		}


		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
			LOG("Received parent \"" + string(parent->mName.C_Str()) + "\". Combining imported transformations from scene graph:");
		#endif

		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
			LOG("Spewing transform hierarchy:");
			aiNode* debug = parent;
			while (debug != nullptr)
			{
				LOG("-> " + string(debug->mName.C_Str()));

				// NOTE: Assimp matrices are stored in row major order
				LOG("\t\t" + to_string(debug->mTransformation.a1) + " " + to_string(debug->mTransformation.a2) + " " + to_string(debug->mTransformation.a3) + " " + to_string(debug->mTransformation.a4));
				LOG("\t\t" + to_string(debug->mTransformation.b1) + " " + to_string(debug->mTransformation.b2) + " " + to_string(debug->mTransformation.b3) + " " + to_string(debug->mTransformation.b4));
				LOG("\t\t" + to_string(debug->mTransformation.c1) + " " + to_string(debug->mTransformation.c2) + " " + to_string(debug->mTransformation.c3) + " " + to_string(debug->mTransformation.c4));
				LOG("\t\t" + to_string(debug->mTransformation.d1) + " " + to_string(debug->mTransformation.d2) + " " + to_string(debug->mTransformation.d3) + " " + to_string(debug->mTransformation.d4));

				debug = debug->mParent;
			}
			LOG("End of transform hierarchy!");
		#endif

		aiMatrix4x4 combinedTransform;	// Start with the identity matrix
		aiNode* current = parent;
		while (current != nullptr && current != scene->mRootNode)
		{
			string currentName = string(current->mName.C_Str());

			if (skipPostRotations && currentName.find("_Post") != string::npos) // HACK: Seems if we skip "_PostRotation" nodes, the directional light orientation will be correct. But, we need this for camera xforms...
			{
				#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
					LOG("\t\tSkipped node \"" + currentName + "\"");
				#endif

				current = current->mParent;
				continue;
			}

			if (currentName.find("$AssimpFbx$") != string::npos)
			{
				#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
					LOG("\t\tCombined node with \"" + currentName + "\"");
				#endif	
				combinedTransform = current->mTransformation * combinedTransform;
			}
			else
			{
				#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
					LOG("\tNode \"" + currentName + "\" is not a transform. Stopping!");
				#endif
				break;
			}

			current = current->mParent;
		}

		return combinedTransform;
	}


	aiNode* BlazeEngine::SceneManager::FindNodeContainingName(aiScene const* scene, string name)
	{
		aiNode* result = nullptr;
		if (result = scene->mRootNode->FindNode(name.c_str()))
		{
			return result;
		}

		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (result = FindNodeRecursiveHelper(scene->mRootNode, name))
		{
			return result;
		}

		LOG("Could not find any node containing the name \"" + name + "\" in the scene graph. Returning nullptr");
		return nullptr;
	}


	aiNode* BlazeEngine::SceneManager::FindNodeRecursiveHelper(aiNode* rootNode, string name)
	{
		if (rootNode == nullptr)
		{
			return nullptr;
		}

		string currentName(rootNode->mName.C_Str());
		std::transform(currentName.begin(), currentName.end(), currentName.begin(), ::tolower);
		if (currentName.find(name) != string::npos)
		{
			LOG("Found node containing \"" + name + "\", returning node \"" + currentName +"\"");
			return rootNode;
		}

		for (unsigned int i = 0; i < rootNode->mNumChildren; i++)
		{
			aiNode* result = FindNodeRecursiveHelper(rootNode->mChildren[i], name);

			if (result != nullptr)
			{
				LOG("Found child node containing \"" + name + "\", returning node \"" + string(result->mName.C_Str()) + "\"");
				return result;
			}
		}
		return nullptr;
	}


	void BlazeEngine::SceneManager::ImportLightsFromScene(aiScene const* scene)
	{
		int numLights = scene->mNumLights;
		if (numLights <= 0)
		{
			LOG_ERROR("Scene has no lights to import!");
			return;
		}
		else
		{
			LOG("Found " + to_string(numLights) + " scene lights");
		}		
		
		bool foundDirectional	= false;	// TEMP: Only find the first directional light
		bool foundAmbient		= false;	// Track: Have we found a point light with a name containing "ambient" yet?

		for (unsigned int i = 0; i < scene->mNumLights; i++)
		{
			switch (scene->mLights[i]->mType)
			{
			case aiLightSource_DIRECTIONAL:
			{
				if (!foundDirectional)
				{
					foundDirectional = true;
					string lightName = string(scene->mLights[i]->mName.C_Str());

					#if defined(DEBUG_SCENEMANAGER_LIGHT_LOGGING)
						LOG("\nFound a directional light \"" + lightName + "\"");
					#endif

					aiMatrix4x4 lightTransform;
					aiNode* current = nullptr;
					if (current = scene->mRootNode->FindNode(scene->mLights[i]->mName))
					{
						#if defined(DEBUG_SCENEMANAGER_LIGHT_LOGGING)
							LOG("Found a corresponding light node in the scene graph...");
						#endif
						lightTransform = GetCombinedTransformFromHierarchy(scene, current->mParent);
						lightTransform = lightTransform * current->mTransformation;	

						#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
							LOG("-> " + string(current->mName.C_Str()) + " (Light's first transformation node)");
							LOG(to_string(current->mTransformation.a1) + " " + to_string(current->mTransformation.a2) + " " + to_string(current->mTransformation.a3) + " " + to_string(current->mTransformation.a4) );
							LOG(to_string(current->mTransformation.b1) + " " + to_string(current->mTransformation.b2) + " " + to_string(current->mTransformation.b3) + " " + to_string(current->mTransformation.b4));
							LOG(to_string(current->mTransformation.c1) + " " + to_string(current->mTransformation.c2) + " " + to_string(current->mTransformation.c3) + " " + to_string(current->mTransformation.c4));
							LOG(to_string(current->mTransformation.d1) + " " + to_string(current->mTransformation.d2) + " " + to_string(current->mTransformation.d3) + " " + to_string(current->mTransformation.d4));
						#endif
					}
					
					vec3 lightColor(scene->mLights[i]->mColorDiffuse.r, scene->mLights[i]->mColorDiffuse.g, scene->mLights[i]->mColorDiffuse.b);

					currentScene->keyLight = Light
					(
						lightName, 
						LIGHT_DIRECTIONAL, 
						lightColor,
						nullptr
					);

					InitializeTransformValues(lightTransform, &currentScene->keyLight.GetTransform());

					Bounds sceneWorldBounds = currentScene->WorldSpaceSceneBounds();
					Bounds transformedBounds = sceneWorldBounds.GetTransformedBounds(glm::inverse(currentScene->keyLight.GetTransform().Model()));
					
					//LOG("Scene world bounds: " + to_string(sceneWorldBounds.xMin) + " < X < " + to_string(sceneWorldBounds.xMax) + ", " + to_string(sceneWorldBounds.yMin) + " < Y < " + to_string(sceneWorldBounds.yMax) + ", " + to_string(sceneWorldBounds.zMin) + " < Z < " + to_string(sceneWorldBounds.zMax));
					//LOG("Framing light frustum about transformed scene bounds:\n\t" + to_string(transformedBounds.xMin) + " < X < " + to_string(transformedBounds.xMax) + ", " + to_string(transformedBounds.yMin) + " < Y < " + to_string(transformedBounds.yMax) + ", " + to_string(transformedBounds.zMin) + " < Z < " + to_string(transformedBounds.zMax));

					ShadowMap* keyLightShadowMap = new ShadowMap	// TEMP: We assume the key light will ALWAYS have a shadow
					(
						lightName,
						CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapWidth,
						CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapHeight,

						-transformedBounds.zMax, // We negate these values because glm::ortho reverses the sign to be "helpful"
						-transformedBounds.zMin,

						&currentScene->keyLight.GetTransform(),
						vec3(0, 0, 0),
						transformedBounds.xMin,
						transformedBounds.xMax,
						transformedBounds.yMin,
						transformedBounds.yMax
					);

					currentScene->keyLight.AddShadowMap(keyLightShadowMap);


					// Note: Assimp seems to import directional lights with their "forward" vector pointing in the opposite direction.
					// This is ok, since we use "forward" as "vector pointing towards the light" when uploading to our shaders...
					#if defined(DEBUG_SCENEMANAGER_LIGHT_LOGGING)
						LOG("Directional light color: " + to_string(lightColor.r) + ", " + to_string(lightColor.g) + ", " + to_string(lightColor.b));
						LOG("Directional light position = " + to_string(currentScene->keyLight.GetTransform().Position().x) + ", " + to_string(currentScene->keyLight.GetTransform().Position().y) + ", " + to_string(currentScene->keyLight.GetTransform().Position().z));
						LOG("Directional light rotation = " + to_string(currentScene->keyLight.GetTransform().GetEulerRotation().x) + ", " + to_string(currentScene->keyLight.GetTransform().GetEulerRotation().y) + ", " + to_string(currentScene->keyLight.GetTransform().GetEulerRotation().z) + " (radians)");
						LOG("Directional light rotation = " + to_string(currentScene->keyLight.GetTransform().GetEulerRotation().x * (180.0f / glm::pi<float>()) ) + ", " + to_string(currentScene->keyLight.GetTransform().GetEulerRotation().y * (180.0f / glm::pi<float>())) + ", " + to_string(currentScene->keyLight.GetTransform().GetEulerRotation().z * (180.0f / glm::pi<float>())) + " (degrees)");
						LOG("Directional light forward = " + to_string(currentScene->keyLight.GetTransform().Forward().x) + ", " + to_string(currentScene->keyLight.GetTransform().Forward().y) + ", " + to_string(currentScene->keyLight.GetTransform().Forward().z));
					#endif
				}
				else
				{
					LOG_ERROR("Found additional directional light. More than 1 directional light is not yet supported!");
				}
			}
				break;

			case aiLightSource_POINT: // Can be either a point or ambient light
			{
				string lightName = string(scene->mLights[i]->mName.C_Str());
				if (!foundAmbient && lightName.find("ambient") != string::npos)	// NOTE: The word "ambient" must appear in the ambient light's name
				{
					foundAmbient = true;

					currentScene->ambientLight = vec3(scene->mLights[i]->mColorDiffuse.r, scene->mLights[i]->mColorDiffuse.g, scene->mLights[i]->mColorDiffuse.b);
					
					#if defined(DEBUG_SCENEMANAGER_LIGHT_LOGGING)
						LOG("Created ambient light from \"" + lightName +"\"");
					#endif
				}
				else
				{
					LOG_ERROR("Found a point light. Point lights are not yet supported!");
				}
			}				
				break;

			case aiLightSource_SPOT:
				LOG_ERROR("Found a spot light. Spot lights are not yet supported!");
				break;

			case aiLightSource_UNDEFINED:
				LOG_ERROR("Found an undefined light type");
				break;

			default:
				LOG_ERROR("Found an unhandled light type");
				break;

			}

			#if defined(DEBUG_SCENEMANAGER_LIGHT_LOGGING)
				LOG("mAngleInnerCone = " + to_string(scene->mLights[i]->mAngleInnerCone) + " radians");
				LOG("mAngleOuterCone = " + to_string(scene->mLights[i]->mAngleOuterCone) + " radians");
				LOG("mAttenuationConstant = " + to_string(scene->mLights[i]->mAttenuationConstant));
				LOG("mAttenuationLinear = " + to_string(scene->mLights[i]->mAttenuationLinear));
				LOG("mAttenuationQuadratic = " + to_string(scene->mLights[i]->mAttenuationQuadratic));
				LOG("mColorAmbient = " + to_string(scene->mLights[i]->mColorAmbient.r) + ", " + to_string(scene->mLights[i]->mColorAmbient.g) + ", " + to_string(scene->mLights[i]->mColorAmbient.b));
				LOG("mColorDiffuse = " + to_string(scene->mLights[i]->mColorDiffuse.r) + ", " + to_string(scene->mLights[i]->mColorDiffuse.g) + ", " + to_string(scene->mLights[i]->mColorDiffuse.b));
				LOG("mColorSpecular = " + to_string(scene->mLights[i]->mColorSpecular.r) + ", " + to_string(scene->mLights[i]->mColorSpecular.g) + ", " + to_string(scene->mLights[i]->mColorSpecular.b));
				LOG("mDirection = " + to_string(scene->mLights[i]->mDirection.x) + ", " + to_string(scene->mLights[i]->mDirection.y) + ", " + to_string(scene->mLights[i]->mDirection.z));
				LOG("mPosition = " + to_string(scene->mLights[i]->mPosition.x) + ", " + to_string(scene->mLights[i]->mPosition.y) + ", " + to_string(scene->mLights[i]->mPosition.z));
				LOG("mSize = " + to_string(scene->mLights[i]->mSize.x) + ", " + to_string(scene->mLights[i]->mSize.y));
				LOG("mUp = " + to_string(scene->mLights[i]->mUp.x) + ", " + to_string(scene->mLights[i]->mUp.y) + ", " + to_string(scene->mLights[i]->mUp.z));
			#endif
		}
	}


	void BlazeEngine::SceneManager::ImportCamerasFromScene(aiScene const* scene /*= nullptr*/, bool clearCameras /*= false*/) // If scene == nullptr, create a camera at the origin
	{
		if (clearCameras)
		{
			currentScene->ClearCameras();
		}		

		if (scene == nullptr) // Signal to create a default camera at the origin
		{
			LOG("Creating a default camera");
			currentScene->RegisterCamera(CAMERA_TYPE_MAIN, new Camera("defaultCamera"));
			currentScene->GetMainCamera()->Initialize
			(
				CoreEngine::GetCoreEngine()->GetConfig()->GetAspectRatio(),
				CoreEngine::GetCoreEngine()->GetConfig()->mainCam.defaultFieldOfView,
				CoreEngine::GetCoreEngine()->GetConfig()->mainCam.defaultNear,
				CoreEngine::GetCoreEngine()->GetConfig()->mainCam.defaultFar
			);
			return;
		}

		string cameraName = string(scene->mCameras[0]->mName.C_Str());

		int numCameras = scene->mNumCameras;
		if (numCameras > 1)
		{
			LOG_ERROR("Found " + to_string(numCameras) + " cameras in the scene. Currently, only 1 camera is supported. Setting the FIRST camera received camera as the main camera.");
		}
		else
		{
			LOG("Found " + to_string(numCameras) + " scene camera: \"" + cameraName + "\"");
		}

		// If we made it this far, we're importing a camera from the scene:
		Camera* newCamera = new Camera(cameraName);

		currentScene->RegisterCamera(CAMERA_TYPE_MAIN, newCamera); // For now, only importing the main camera is supported...



		// TODO: In the current version of Assimp, camera import is broken.
		// + The mLookAt, mUp vectors seem to just be the world forward, up vectors, and mTransformation is the identity...
		// + Importing cameras facing towards Z+ results in a flipped camera?
		// + Importing cameras facing towards Z- results in an extra +90 degree rotation about Y being applied?


		aiNode* camNode = scene->mRootNode->FindNode(scene->mCameras[0]->mName);
		if (camNode)
		{			
			#if defined(DEBUG_SCENEMANAGER_CAMERA_LOGGING) || defined(DEBUG_TRANSFORMS)
				LOG("-> " + cameraName + " (Camera's first transformation node)");
				LOG(to_string(camNode->mTransformation.a1) + " " + to_string(camNode->mTransformation.a2) + " " + to_string(camNode->mTransformation.a3) + " " + to_string(camNode->mTransformation.a4));
				LOG(to_string(camNode->mTransformation.b1) + " " + to_string(camNode->mTransformation.b2) + " " + to_string(camNode->mTransformation.b3) + " " + to_string(camNode->mTransformation.b4));
				LOG(to_string(camNode->mTransformation.c1) + " " + to_string(camNode->mTransformation.c2) + " " + to_string(camNode->mTransformation.c3) + " " + to_string(camNode->mTransformation.c4));
				LOG(to_string(camNode->mTransformation.d1) + " " + to_string(camNode->mTransformation.d2) + " " + to_string(camNode->mTransformation.d3) + " " + to_string(camNode->mTransformation.d4));
			#endif
			
			aiMatrix4x4 camTransform = GetCombinedTransformFromHierarchy(scene, camNode->mParent, false);
			camTransform = camTransform * camNode->mTransformation;
			 
			InitializeTransformValues(camTransform, newCamera->GetTransform());
		}

		vec3 camPosition = newCamera->GetTransform()->Position();

		#if defined(DEBUG_SCENEMANAGER_CAMERA_LOGGING)
			
			vec3 camRotation = newCamera->GetTransform()->GetEulerRotation();

			LOG("Camera is located at " + to_string(camPosition.x) + " " + to_string(camPosition.y) + " " + to_string(camPosition.z) + ". Near = " + to_string(scene->mCameras[0]->mClipPlaneNear) + ", " + "far = " + to_string(scene->mCameras[0]->mClipPlaneFar) );
			LOG("Camera rotation is " + to_string(camRotation.x) + " " + to_string(camRotation.y) + " " + to_string(camRotation.z) + " (radians)");
			LOG("Camera rotation is " + to_string(camRotation.x * (180.0f / glm::pi<float>())) + " " + to_string(camRotation.y * (180.0f / glm::pi<float>())) + " " + to_string(camRotation.z * (180.0f / glm::pi<float>())) + " (degrees)");
		#endif

		LOG_ERROR("Camera field of view is NOT currently loaded from the source file. A hard-coded default value is used for now");

		newCamera->Initialize
		(
			(float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes / (float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
			CoreEngine::GetCoreEngine()->GetConfig()->mainCam.defaultFieldOfView, //scene->mCameras[0]->mHorizontalFOV; // TODO: Implement this (Needs to be converted to a vertical FOV???)
			scene->mCameras[0]->mClipPlaneNear,
			scene->mCameras[0]->mClipPlaneFar,
			nullptr,
			camPosition
		);
	}
}


