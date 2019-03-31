#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"

#include "assimp/Importer.hpp"	// Importer interface
#include "assimp/scene.h"		// Output data structure
#include "assimp/postprocess.h"	// Post processing flags


//// DEBUG:
//#include <iostream>
//using std::cout;
//using std::to_string;


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


	void SceneManager::LoadScene(string scenePath)
	{
		if (currentScene)
		{
			// TO DO: Write a destructor/cleanup correctly when deleting a scene
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, this, new string("WARNING: Current scene already exists. Debug deallocation is to just delete it, but this is likely leaking memory!") });
			delete currentScene;
		}
		currentScene = new Scene();


		
		// Load our .FBX:
		// ...

		// Create a new game object for every item in the .FBX:
		// ...
		// TO DO: Guard file loading flow with exception handling



		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(scenePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		
		if (!scene)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ERROR, nullptr, new string("Failed to load texture at " + scenePath + ": " + importer.GetErrorString() ) });
			return;
		}
		else
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Successfully Loaded " + scenePath) });
		}


		if (scene->HasTextures())
		{
			int numTextures = scene->mNumTextures;

			cout << "Scene has " << numTextures << " textures!\n";

			if (scene->mTextures[0]->CheckFormat("png"))
			{
				cout << "Found a png: " << scene->mTextures[0]->mWidth << " x " << scene->mTextures[0]->mHeight << "\n";
			}
			else
			{
				cout << "Did not find a png\n";
			}
		}
		string texPath = CoreEngine::GetCoreEngine()->GetConfig()->scene.sceneRoot + "testGrid.png";
		const aiScene* testImage = importer.ReadFile(texPath, 0);

		if (testImage)
		{
			cout << "LOADED TEST IMAGE\n";
		}
		else
			cout << "NOT\n";


		
		


		// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:


		// Create a material and shader:
		unsigned int shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);

		Material* newMaterial = new Material("testMaterial1", shaderIndex);

		// Create textures and assign them to the material:
		Texture* testAlbedo = Texture::LoadTextureFromPath("./debug/invalid/albedo/path");
		newMaterial->SetTexture(testAlbedo, TEXTURE_ALBEDO);
		testAlbedo->Fill(vec4(1, 1, 1, 1));

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
		//testAlbedo->Fill(vec4(0, 1, 0, 1));
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



	// Shader management:		
	//*******************

	unsigned int SceneManager::GetShaderIndexFromShaderName(string shaderName, bool findExisting)
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


