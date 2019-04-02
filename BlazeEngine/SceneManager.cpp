#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"

#include "assimp/Importer.hpp"	// Importer interface
#include "assimp/scene.h"		// Output data structure
#include "assimp/postprocess.h"	// Post processing flags

#define STB_IMAGE_IMPLEMENTATION	// Only include this define ONCE in the project
#include "stb_image.h"				// STB image loader

//// DEBUG:
//#include <iostream>
//using std::cout;
//using std::to_string;



namespace BlazeEngine
{
	SceneManager::SceneManager() : EngineComponent("SceneManager")
	{
		stbi_set_flip_vertically_on_load(true);	// Tell stb_image to flip the y-axis on loading (So pixel (0,0) is in the bottom-left of the image)
	}

	SceneManager::~SceneManager()
	{
		
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

		// Initialize shaders:
		shaders = new Shader*[MAX_SHADERS];
		for (unsigned int i = 0; i < MAX_SHADERS; i++)
		{
			shaders[i] = nullptr;
		}
		currentShaderCount = 0;

		// Initialize our Shaders to match the order of the SHADER enum:
		// Load error shader (Shader index 0):
		int loadedShaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName);
		if (loadedShaderIndex != 0 || shaders[0] == nullptr || currentShaderCount != 1)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Shader 0 (Error shader) could not be loaded!") });
		}
		// Load default shader (Shader index 1):
		loadedShaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);
		if (loadedShaderIndex != 1 || shaders[1] == nullptr || currentShaderCount != 2)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, this, new string("Warning: Shader 1 (Default shader) could not be loaded!") });
		}

		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Scene manager started!") });
	}


	void SceneManager::Shutdown()
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Scene manager shutting down...") });

		// Cleanup the scene:
		for (int i = 0; i < (int)currentScene->meshes.size(); i++)
		{
			currentScene->meshes.at(i).DestroyMesh();
			/*if (currentScene->meshes[i].Vertices())
			{
				delete currentScene->meshes[i].Vertices();
			}

			if (currentScene->meshes[i].Indices())
			{
				delete currentScene->meshes[i].Indices();
			}*/
		}

		for (int i = 0; i < currentScene->gameObjects.size(); i++)
		{
			if (currentScene->gameObjects.at(i))
			{
				delete currentScene->gameObjects.at(i);
			}
		}

		if (currentScene)
		{
			delete currentScene;
		}
		
		if (materials)
		{
			for (unsigned int i = 0; i < MAX_MATERIALS; i++)
			{
				if (materials[i])
				{
					delete materials[i];
				}
			}
			delete[] materials;
			currentMaterialCount = 0;
		}		

		for (unsigned int i = 0; i < currentShaderCount; i++)
		{
			if (shaders[i])
			{
				glDeleteProgram(shaders[i]->ShaderReference());
				delete shaders[i];
			}
		}
		delete[] shaders;
		currentShaderCount = 0;
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
		if (currentScene)
		{
			// TO DO: Write a destructor/cleanup correctly when deleting a scene
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, this, new string("WARNING: A scene already currently exists. Debug handling is to just delete it, but this is likely leaking memory!") });
			delete currentScene;
		}
		
		currentScene = new Scene();

		// Assemble paths:
		string sceneRoot = CoreEngine::GetCoreEngine()->GetConfig()->scene.sceneRoot + sceneName + "\\";
		string fbxPath = sceneRoot + sceneName + ".fbx";

		// Load our .fbx using Assimp:
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fbxPath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if (!scene)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, nullptr, new string("Failed to load scene: " + fbxPath + ": " + importer.GetErrorString() ) });
			return;
		}
		else
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Successfully loaded scene: " + fbxPath) });
		}
		
		// Extract materials and textures:
		if (scene->HasMaterials())
		{
			int numMaterials = scene->mNumMaterials;
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Found " + to_string(numMaterials) + " scene materials" ) });

			// Create Blaze Engine materials:
			for (int i = 0; i < numMaterials; i++)
			{
				// Get the material name:
				aiString name;
				scene->mMaterials[i]->Get(AI_MATKEY_NAME, name);

				CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Creating material: " + string(name.C_Str())) });

				// Create a shader:
				unsigned int shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);
				// TO DO: Figure out how to load different shaders based on AI_MATKEY_SHADING_MODEL

				// Create a Blaze Engine material:
				Material* newMaterial = new Material(string(name.C_Str()), shaderIndex);

				// Extract material's textures:
				if (scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE) > 0) // If there is more than 1 texture in the slot, we only get the FIRST...
				{
					CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Material " + to_string(i) + ": Loading diffuse texture...") });

					aiString path;
					scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path); // We only get the texture at index 0 (any others are ignored...)

					if (path.length > 0)
					{
						string texturePath = sceneRoot + string(path.C_Str());
						CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Found texture path: " + texturePath) });

						// Find the texture if it has already been loaded, or load it otherwise:
						Texture* diffuseTexture = FindLoadTextureByPath(texturePath);


						// Add texture to a material:
						newMaterial->SetTexture(diffuseTexture, TEXTURE_ALBEDO);


						// Add the material to our material list:
						unsigned int materialIndex = AddMaterial(newMaterial, false);
					}
					else
					{
						CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Material does not contain a diffuse texture path") });
					}					
				}
			}

			// TO DO: Load lights, cameras, geometry + materials

			


		}
		else
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Scene has no materials") });
		}



		// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:


		
		// Create 1st cube using test texture:

		Material* newMaterial = GetMaterial("cubePhong");

		// -> Already has an albedo...

		Texture* testNormal = Texture::LoadTextureFromPath(".debug/invalid/normal/path");
		newMaterial->SetTexture(testNormal, TEXTURE_NORMAL);
		testNormal->Fill(vec4(0, 0, 1, 1));
		
		Texture* testRoughness = Texture::LoadTextureFromPath(".debug/invalid/rough/path");
		newMaterial->SetTexture(testRoughness, TEXTURE_ROUGHNESS);
		testRoughness->Fill(vec4(1, 1, 0, 1));

		Texture* testMetallic = Texture::LoadTextureFromPath(".debug/invalid/metal/path");
		newMaterial->SetTexture(testMetallic, TEXTURE_METALLIC);
		testMetallic->Fill(vec4(0, 1, 1, 1));

		Texture* testAmbientOcclusion = Texture::LoadTextureFromPath(".debug/invalid/AO/path");
		newMaterial->SetTexture(testAmbientOcclusion, TEXTURE_AMBIENT_OCCLUSION);
		testAmbientOcclusion->Fill(vec4(0.5, 0.5, 0.5, 1));

		// -> Material already in material list...

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		Mesh mesh = Mesh::CreateCube();
		mesh.MaterialIndex() = GetMaterialIndex("cubePhong");
		// -> Get using string instead of index...

		currentScene->meshes.push_back(mesh);
		int meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address

		// Assemble a list of all meshes held by a Renderable:
		vector<Mesh*> viewMeshes;
		viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
		Renderable testRenderable(viewMeshes);

		// Construct a GameObject for the cube:
		GameObject* cubeObject = new GameObject("cubeObject1", testRenderable);

		cubeObject->GetTransform()->SetPosition(vec3(-3, 0,0));
		

		// Add cube object to scene:
		currentScene->gameObjects.push_back(cubeObject);
		int gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());



		// Assemble a second cube:

		// Create a material and shader:
		int shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);

		newMaterial = new Material("testMaterial2", shaderIndex);

		// Create textures and assign them to the material:
		Texture* testAlbedo = new Texture(256, 256, false); // Create a new, unfilled texture
		testAlbedo->Fill(vec4(1, 1, 1, 1), vec4(0, 0, 1, 1), vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
		
		//Texture* testAlbedo = Texture::LoadTextureFromPath("INVALID PATH");  // Bright red error texture
		
		newMaterial->SetTexture(testAlbedo, TEXTURE_ALBEDO);

		// Add the material to our material list:
		int materialIndex = AddMaterial(newMaterial);

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		mesh = Mesh::CreateCube();
		mesh.MaterialIndex() = materialIndex;

		currentScene->meshes.push_back(mesh);
		meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address

		// Assemble a list of all meshes held by a Renderable:
		viewMeshes.clear();
		viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
		testRenderable = Renderable(viewMeshes);

		// Construct a GameObject for the cube:
		cubeObject = new GameObject("cubeObject2", testRenderable);

		cubeObject->GetTransform()->SetPosition(vec3(3, 0, 0));
		cubeObject->GetTransform()->SetEulerRotation(vec3(0.5f,0.5f,0.5f));


		// Add cube object to scene:
		currentScene->gameObjects.push_back(cubeObject);
		gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());


		




		// Assemble a third cube:

		// Create a material and shader:
		shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName); // Use the error shader

		newMaterial = new Material("testMaterial3", shaderIndex);

		// Create textures and assign them to the material:
		testAlbedo = Texture::LoadTextureFromPath("./another/invalid/path");
		newMaterial->SetTexture(testAlbedo, TEXTURE_ALBEDO);

		// Add the material to our material list:
		materialIndex = AddMaterial(newMaterial);

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		mesh = Mesh::CreateCube();
		mesh.MaterialIndex() = materialIndex;

		currentScene->meshes.push_back(mesh);
		meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address

		// Assemble a list of all meshes held by a Renderable:
		viewMeshes.clear();
		viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
		testRenderable = Renderable(viewMeshes);

		// Construct a GameObject for the cube:
		cubeObject = new GameObject("cubeObject2", testRenderable);

		cubeObject->GetTransform()->SetPosition(vec3(0, -5, -5));
		cubeObject->GetTransform()->SetEulerRotation(vec3(-0.3f, 0.3f, -0.3f));


		// Add cube object to scene:
		currentScene->gameObjects.push_back(cubeObject);
		gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());



			   


		// Assemble material mesh lists:
		AssembleMaterialMeshLists();



		// Set up lights:
		currentScene->ambientLight = vec4(0.5, 0.5, 0.5, 1.0f);
		currentScene->keyLight = Light(LIGHT_DIRECTIONAL, vec4(1.0f, 1.0f, 1.0f, 1.0f), 1.0f);
		currentScene->keyLight.GetTransform().Rotate(vec3(3.14f/4.0f, 3.14f/8.0f, 0)); // Rotation in radians
		currentScene->keyLight.SetColor(vec4(0,1,0,1));
		currentScene->keyLight.SetIntensity(2.0f);

		// Set up a player object:
		PlayerObject* player = new PlayerObject();
		currentScene->gameObjects.push_back(player);
		currentScene->mainCamera = player->GetCamera();
		currentScene->mainCamera->Initialize(
			vec3(0, 0, 0),
			(float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes / (float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
			CoreEngine::GetCoreEngine()->GetConfig()->viewCam.fieldOfView,
			CoreEngine::GetCoreEngine()->GetConfig()->viewCam.near,
			CoreEngine::GetCoreEngine()->GetConfig()->viewCam.far
			);

		player->GetTransform()->SetPosition(vec3(0.0f, 0.0f, 4.0f));
		
	}


	Material * BlazeEngine::SceneManager::GetMaterial(string materialName)
	{
		for (unsigned int i = 0; i < currentMaterialCount; i++)
		{
			if (materials[i]->Name() == materialName)
			{
				return materials[i];
			}
		}

		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, this, new string("Could not find material \"" + materialName + "\"") });
		return nullptr;
	}

	
	int SceneManager::GetMaterialIndex(string materialName)
	{
		// Check if a material with the same name exists, and return it if it does:
		int materialIndex = FindMaterialIndex(materialName);
		if (materialIndex != -1)
		{
			return materialIndex;
		}
		
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, this, new string("Material \"" + materialName + "\" does not exist... Creating a new material with default shader") });

		// If we've made it this far, no material with the given name exists. Create it:
		Material* newMaterial = new Material(materialName, GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName)); // Assign the default shader

		return AddMaterial(newMaterial, false);
	}

	unsigned int SceneManager::AddMaterial(Material* newMaterial, bool checkForExisting) // checkForExisting = true by default
	{
		if (checkForExisting) // Check if a material with the same name exists, and return it if it does
		{
			int materialIndex = FindMaterialIndex(newMaterial->Name());
			if (materialIndex != -1)
			{
				CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Found existing material \"" + materials[materialIndex]->Name() + "\" at index " + to_string(materialIndex)) });
				return materialIndex;
			}
		}			

		// Otherwise, add a new material:
		if (currentMaterialCount == MAX_MATERIALS)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, this, new string("Cannot add any new materials: Max materials have been added! Returning material at index 0") });
			return 0; // Error: Return first material
		}

		materials[currentMaterialCount] = newMaterial;
		unsigned int newIndex = currentMaterialCount;
		currentMaterialCount++;

		return newIndex; // Return the previous index
	}

	int SceneManager::FindMaterialIndex(string materialName)
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
		materialMeshLists.clear();
		materialMeshLists.reserve(MAX_MATERIALS);

		unsigned int numMeshes = 0;
		for (int i = 0; i < (int)currentScene->renderables.size(); i++)
		{
			for (int j = 0; j < (int)currentScene->renderables.at(i)->ViewMeshes()->size(); j++)
			{
				Mesh* viewMesh = currentScene->renderables.at(i)->ViewMeshes()->at(j);
				int materialIndex = viewMesh->MaterialIndex();
				if (materialIndex < 0)
				{
					CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("AssembleMaterialMeshLists() is skipping a mesh with no material!") });
					// TO DO: Assign a default/error material and still add the mesh?
				}
				else
				{
					if (materialIndex >= materialMeshLists.size())
					{
						materialMeshLists.emplace_back(vector<Mesh*>());
						materialMeshLists.at(materialMeshLists.size() - 1).reserve(50); // TO DO: Tune this value based on the actual number of meshes loaded?
					}
					materialMeshLists.at(materialIndex).emplace_back(viewMesh);
					numMeshes++;
				}
			}
		}

		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Finished adding " + to_string(numMeshes) + " meshes for " + to_string(materialMeshLists.size()) + " materials to the material mesh lists") });
	}

	Texture* BlazeEngine::SceneManager::FindLoadTextureByPath(string texturePath)
	{
		for (unsigned int i = 0; i < currentTextureCount; i++)
		{
			if (textures[i] && textures[i]->TexturePath() == texturePath)
			{
				CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Texture at path " + texturePath + " has already been loaded") });
				return textures[i];
			}
		}

		// If we've made it this far, try and load the texture
		return Texture::LoadTextureFromPath(texturePath);
	}


	// Shader management:		
	//*******************

	unsigned int SceneManager::GetShaderIndexFromShaderName(string shaderName, bool findExisting) // findExisting == false by default
	{
		if (findExisting || shaderName == CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName)
		{
			// Return the index if it's found, or load the shader and return a new index, or return the error shader otherwise
			for (unsigned int i = 0; i < MAX_SHADERS; i++)
			{
				if (shaders[i] && shaders[i]->Name() == shaderName)
				{
					return i; // We're done!
				}
			}
		}		

		// If we've made it this far, the shader was not found. Attempt to load it:
		Shader* shader = Shader::CreateShader(shaderName);
		int shaderIndex = -1;
		if (shader != nullptr)
		{
			shaderIndex = currentShaderCount; // Cache the insertion index

			shaders[currentShaderCount] = shader;
			currentShaderCount++;

			return shaderIndex;
		}
		else // If all else fails, return the error shader:
		{
			return 0;
		}
	}
}


