#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	SceneManager::SceneManager() : EngineComponent("SceneManager")
	{
		
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
			if (currentScene->meshes[i].Vertices())
			{
				delete currentScene->meshes[i].Vertices();
			}

			if (currentScene->meshes[i].Indices())
			{
				delete currentScene->meshes[i].Indices();
			}
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


	void SceneManager::LoadScene(string scenePath)
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, this, new string("Could not load " + scenePath + ", as LoadScene() is not implemented. Using a debug hard coded path for now!") });

		if (currentScene)
		{
			// TO DO: Write a destructor/cleanup correctly when deleting a scene
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, this, new string("WARNING: Current scene already exists. Debug deallocation is to just delete it, but this is likely leaking memory!") });
			delete currentScene;
		}
		currentScene = new Scene();


		// Flush any existing scene objects: (NOTE: Any objects that access these must be shut down first!)
		// TO DO: Make sure we're deallocating everything before clearing the lists
		//gameObjects.clear();
		/*renderables.clear();*/
		/*meshes.clear();*/  // TO DO: delete meshes.Vertices()
		/*materials.clear();*/
		/*forwardLights.clear();
		mainCamera = Camera();*/
		
		// Load our .FBX:
		// ...

		// Create a new game object for every item in the .FBX:
		// ...
		// TO DO: Guard file loading flow with exception handling




		// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:


		// Create a material and shader:
		unsigned int shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);

		Material* newMaterial = new Material("testMaterial1", shaderIndex);

		// Create textures and assign them to the material:
		Texture* testAlbedo = Texture::LoadTextureFromPath("./debug/invalid/path");
		newMaterial->SetTexture(testAlbedo, TEXTURE_ALBEDO);

		// Add the material to our material list:
		unsigned int materialIndex = AddMaterial(newMaterial);

		// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		Mesh mesh = Mesh::CreateCube();
		mesh.MaterialIndex() = materialIndex;

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
		shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);

		newMaterial = new Material("testMaterial2", shaderIndex);

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

		cubeObject->GetTransform()->SetPosition(vec3(3, 0, 0));
		cubeObject->GetTransform()->SetEulerRotation(vec3(0.5f,0.5f,0.5f));


		// Add cube object to scene:
		currentScene->gameObjects.push_back(cubeObject);
		gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());



		// Assemble a third cube:

		// Create a material and shader:
		shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);

		newMaterial = new Material("testMaterial2", shaderIndex);

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

	
	int SceneManager::GetMaterial(string materialName)
	{
		// Check if a material with the same name exists, and return it if it does:
		int materialIndex = FindMaterialIndex(materialName);
		if (materialIndex != -1)
		{
			return materialIndex;
		}
		
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
		materialMeshLists.resize(MAX_MATERIALS);
		for (int i = 0; i < (int)currentScene->renderables.size(); i++)
		{
			for (int j = 0; j < (int)currentScene->renderables.at(i)->ViewMeshes()->size(); j++)
			{
				Mesh* viewMesh = currentScene->renderables.at(i)->ViewMeshes()->at(j);
				int materialIndex = viewMesh->MaterialIndex();
				if (materialIndex < 0)
				{
					CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("AssembleMaterialMeshLists() is skipping a mesh with no material!") });

				}
				else
				{
					materialMeshLists.at(materialIndex).emplace_back(viewMesh);
				}
			}
		}

		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Finished adding " + to_string(materialMeshLists.size()) + " meshes to material mesh lists") });
	}



	// Shader management:		
	//*******************

	unsigned int SceneManager::GetShaderIndexFromShaderName(string shaderName)
	{
		// Return the index if it's found, or load the shader and return a new index, or return the error shader otherwise
		int shaderIndex = -1;
		for (unsigned int i = 0; i < MAX_SHADERS; i++)
		{
			if (shaders[i] && shaders[i]->Name() == shaderName)
			{
				return i; // We're done!
			}
		}

		// If we've made it this far, the shader was not found. Attempt to load it:
		Shader* shader = Shader::CreateShader(shaderName);

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


