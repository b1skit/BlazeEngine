#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#include "assimp/Importer.hpp"	// Importer interface
//#include "assimp/scene.h"		// Output data structure
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

		LOG("Scene manager started!");
	}


	void SceneManager::Shutdown()
	{
		LOG("Scene manager shutting down...");

		// Cleanup the scene:
		for (int i = 0; i < (int)currentScene->meshes.size(); i++)
		{
			currentScene->meshes.at(i).DestroyMesh();
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
					if (materials[i]->GetShader() != nullptr)
					{
						glDeleteProgram(materials[i]->GetShader()->ShaderReference());
						delete materials[i]->GetShader();
					}
					delete materials[i];
				}
			}
			delete[] materials;
			currentMaterialCount = 0;
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
		if (currentScene)
		{
			// TO DO: Write a destructor/cleanup correctly when deleting a scene
			LOG("DEBUG: WARNING: A scene already currently exists. Debug handling is to just delete it, but this is likely leaking memory!");
			delete currentScene;
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
		); // aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials

		if (!scene)
		{
			CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_ENGINE_QUIT, nullptr, new string("Failed to load scene file: " + fbxPath + ": " + importer.GetErrorString() ) });
			return;
		}
		else
		{
			LOG("Successfully loaded scene file " + fbxPath);
		}


		//// DEBUG:
		//BuildSceneObjects(scene->mRootNode, scene);


		//for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		//{
		//	LOG("Found a mesh named " + string(scene->mMeshes[i]->mName.C_Str()) + " length = " + to_string(scene->mMeshes[i]->mName.length) );

		//	aiNode* test = nullptr;
		//	test = scene->mRootNode->FindNode(scene->mMeshes[i]->mName.C_Str());
		//	if (test)
		//	{
		//		LOG("FOUND MATCHING NODE!");
		//		LOG("has " + to_string(test->mNumMeshes));
		//	}
		//	else
		//	{
		//		LOG("faiiiiiiiiiiiiiiled");
		//	}
		//}
		

		




		// Check for embedded textures:
		if (scene->HasTextures())
		{
			int numTextures = scene->mNumTextures;
			LOG_ERROR("Found " + to_string(numTextures) + " embedded scene textures. These will NOT be loaded!");
		}
		else
		{
			LOG("Scene has no embedded textures");
		}

		
		// Extract materials and textures:
		//--------------------------------
		if (scene->HasMaterials())
		{
			int numMaterials = scene->mNumMaterials;
			LOG("Found " + to_string(numMaterials) + " scene materials");

			// Create Blaze Engine materials:
			for (int i = 0; i < numMaterials; i++)
			{
				// Get the material name:
				aiString name;
				if (AI_SUCCESS == scene->mMaterials[i]->Get(AI_MATKEY_NAME, name))
				{
					aiShadingMode shaderType = aiShadingMode_NoShading;
					if (AI_SUCCESS != scene->mMaterials[i]->Get(AI_MATKEY_SHADING_MODEL, shaderType))
					{
						LOG_ERROR("Couldn't load shader type!!!");
					}

					LOG("Creating material: \"" + string(name.C_Str()) + "\" of shader type \"" + to_string(shaderType) + "\"");

					// Create a Blaze Engine material:
					string matName = string(name.C_Str());
					Material* newMaterial = new Material(matName, CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);

					// Extract material's textures:
					Texture* diffuseTexture = nullptr;
					if (scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE) > 0) // If there is more than 1 texture in the slot, we only get the FIRST...
					{
						int numTextures = scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE);
						LOG("Material has " + to_string(numTextures) + " aiTextureType_DIFFUSE textures...");


						// TO DO: Loop through EVERY texture, and use a switch statement to handle each type


						LOG("Material " + to_string(i) + ": Loading diffuse texture...");

						aiString path;
						scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path); // We only get the texture at index 0 (any others are ignored...)

						if (path.length > 0)
						{
							string texturePath = sceneRoot + string(path.C_Str());
							LOG("Found texture path: " + texturePath);

							// Find the texture if it has already been loaded, or load it otherwise:
							diffuseTexture = FindLoadTextureByPath(texturePath);
						}
						else
						{
							LOG("Material does not contain a diffuse texture path. Assigning an error texture");
						}
					}
					else
					{
						LOG("Material " + to_string(i) + ": No diffuse texture found. Assigning an error texture");
					}

					if (diffuseTexture == nullptr)
					{
						diffuseTexture = FindLoadTextureByPath("errorPath"); // TO DO: Make "errorPath" a global/static string, so it can be reused anywhere?
					}
					// Add texture to a material:
					newMaterial->SetTexture(diffuseTexture, TEXTURE_ALBEDO);



					// Add the material to our material list:
					AddMaterial(newMaterial, false);
				}
			}
		}
		else
		{
			LOG_ERROR("Scene has no materials");
		}


		// Extract meshes:
		//----------------
		if (scene->HasMeshes())
		{
			int numMeshes = scene->mNumMeshes;
			LOG("Found " + to_string(numMeshes) + " scene meshes");

			// Loop through each mesh in the scene
			for (int currentMesh = 0; currentMesh < numMeshes; currentMesh++)
			{
				if (
					scene->mMeshes[currentMesh]->HasPositions()
					&& scene->mMeshes[currentMesh]->HasFaces()
					&& scene->mMeshes[currentMesh]->HasNormals()
					//&& scene->mMeshes[currentMesh]->HasVertexColors(0)
					&& scene->mMeshes[currentMesh]->HasTextureCoords(0)
					//&& scene->mMeshes[currentMesh]->HasTangentsAndBitangents()
					)
				{
					int numVerts		= scene->mMeshes[currentMesh]->mNumVertices;
					int numFaces		= scene->mMeshes[currentMesh]->mNumFaces;
					int numUVs			= scene->mMeshes[currentMesh]->mNumUVComponents[0]; // Just look at the first UV channel for now...
					int numUVChannels	= scene->mMeshes[currentMesh]->GetNumUVChannels();
					int materialIndex	= scene->mMeshes[currentMesh]->mMaterialIndex;

					LOG("Mesh #" + to_string(currentMesh) + " is valid: " + to_string(numVerts) + " vertices, " + to_string(numFaces) + " faces, " + to_string(numUVChannels) + " UV channels, " + to_string(numUVs) + " UV components in channel 0, using material #" + to_string(materialIndex));

					Vertex* vertices = new Vertex[numVerts];
					LOG("Created an array of " + to_string(numVerts) + " vertices!");

					// Fill the vertices array:
					for (int currentVert = 0; currentVert < numVerts; currentVert++)
					{
						vertices[currentVert] = Vertex
						{
							vec3(scene->mMeshes[currentMesh]->mVertices[currentVert].x, scene->mMeshes[currentMesh]->mVertices[currentVert].y, scene->mMeshes[currentMesh]->mVertices[currentVert].z),
							vec3(scene->mMeshes[currentMesh]->mNormals[currentVert].x, scene->mMeshes[currentMesh]->mNormals[currentVert].y, scene->mMeshes[currentMesh]->mNormals[currentVert].z),
							vec4(0.5f, 0.5f ,0.5f , 1.0f), // TO DO: Color - Populate this?
							vec2(scene->mMeshes[currentMesh]->mTextureCoords[0][currentVert].x, scene->mMeshes[currentMesh]->mTextureCoords[0][currentVert].y)
						};
					}

					// Fill the indices array:
					int numIndices = scene->mMeshes[currentMesh]->mNumFaces * 3;
					GLuint* indices = new GLuint[numIndices];
					LOG("Created an array of " + to_string(numIndices) + " indices!");
					
					for (int currentFace = 0; currentFace < numFaces; currentFace++)
					{
						for (int currentIndex = 0; currentIndex < 3; currentIndex++)
						{
							if (scene->mMeshes[currentMesh]->mFaces[currentFace].mNumIndices != 3)
							{
								LOG_ERROR("Found a face that doesn't have 3 indices!")
							}
							indices[(currentFace * 3) + currentIndex] = scene->mMeshes[currentMesh]->mFaces[currentFace].mIndices[currentIndex];

							//LOG("Index # " + to_string((currentFace * 3) + currentIndex) + " = Face #" + to_string(currentFace) + " index #" + to_string(currentIndex) + " = " + to_string(scene->mMeshes[currentMesh]->mFaces[currentFace].mIndices[currentIndex]));
						}
					}

					Mesh newMesh(vertices, numVerts, indices, numIndices, materialIndex);

					currentScene->meshes.push_back(newMesh);
					int meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address				

					// Assemble a list of all meshes held by a Renderable:
					vector<Mesh*> viewMeshes;
					viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
					Renderable testRenderable(viewMeshes);

					// Construct a GameObject for the cube:
					GameObject* meshGameObject = new GameObject("cubeObject1", testRenderable);

					/*cubeObject->GetTransform()->SetPosition(vec3(-3, 0, 0));*/


					// Add cube object to scene:
					currentScene->gameObjects.push_back(meshGameObject);
					int gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

					// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
					currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());


				}
				else
				{
					LOG("Mesh is missing the following properties:");
					if (!scene->mMeshes[currentMesh]->HasPositions())				LOG("\t - positions");
					if (!scene->mMeshes[currentMesh]->HasFaces())					LOG("\t - faces");
					if (!scene->mMeshes[currentMesh]->HasNormals())					LOG("\t - normals");
					if (!scene->mMeshes[currentMesh]->HasVertexColors(0))			LOG("\t - vertex colors");
					if (!scene->mMeshes[currentMesh]->HasTextureCoords(0))			LOG("\t - texture coordinates");
					if (!scene->mMeshes[currentMesh]->HasTangentsAndBitangents())	LOG("\t - tangents & bitangents");
				}
			}

		}
		else
		{
			LOG_ERROR("Scene has no meshes");
		}


		// Extract lights:
		//----------------
		if (scene->HasLights())
		{
			int numLights = scene->mNumLights;
			LOG("Found " + to_string(numLights) + " scene lights");

			// TO DO: Load lights
		}
		else
		{
			LOG_ERROR("Scene has no lights");
		}


		// Extract cameras:
		//-----------------
		if (scene->HasCameras())
		{
			int numCameras = scene->mNumCameras;
			LOG("Found " + to_string(numCameras) + " scene cameras");

			// TO DO: Load cameras
			// Player object should get the main camera, and set its position/orientation as its start point?
		}
		else
		{
			LOG_ERROR("Scene has no cameras");
		}




		//// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:


		//
		//// Create 1st cube using test texture:

		//Material* newMaterial = GetMaterial("cubePhong");

		//// -> Already has an albedo...

		//Texture* testNormal = FindLoadTextureByPath(".debug/invalid/normal/path");
		//newMaterial->SetTexture(testNormal, TEXTURE_NORMAL);
		//testNormal->Fill(vec4(0, 0, 1, 1));
		//
		//Texture* testRoughness = FindLoadTextureByPath(".debug/invalid/rough/path");
		//newMaterial->SetTexture(testRoughness, TEXTURE_ROUGHNESS);
		//testRoughness->Fill(vec4(1, 1, 0, 1));

		//Texture* testMetallic = FindLoadTextureByPath(".debug/invalid/metal/path");
		//newMaterial->SetTexture(testMetallic, TEXTURE_METALLIC);
		//testMetallic->Fill(vec4(0, 1, 1, 1));

		//Texture* testAmbientOcclusion = FindLoadTextureByPath(".debug/invalid/AO/path");
		//newMaterial->SetTexture(testAmbientOcclusion, TEXTURE_AMBIENT_OCCLUSION);
		//testAmbientOcclusion->Fill(vec4(0.5, 0.5, 0.5, 1));

		//// -> Material already in material list...

		//// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		//Mesh mesh = Mesh::CreateCube();
		//mesh.MaterialIndex() = GetMaterialIndex("cubePhong");
		//// -> Get using string instead of index...

		//currentScene->meshes.push_back(mesh);
		//int meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address

		//// Assemble a list of all meshes held by a Renderable:
		//vector<Mesh*> viewMeshes;
		//viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
		//Renderable testRenderable(viewMeshes);

		//// Construct a GameObject for the cube:
		//GameObject* cubeObject = new GameObject("cubeObject1", testRenderable);

		//cubeObject->GetTransform()->SetPosition(vec3(-3, 0,0));
		//

		//// Add cube object to scene:
		//currentScene->gameObjects.push_back(cubeObject);
		//int gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		//// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		//currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());



		//// Assemble a second cube:

		//// Create a material and shader:
		//newMaterial = new Material("testMaterial2", CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);

		///*int shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName);
		//newMaterial = new Material("testMaterial2", shaderIndex);*/

		//// Create textures and assign them to the material:
		//Texture* testAlbedo = new Texture(256, 256, false); // Create a new, unfilled texture
		//testAlbedo->Fill(vec4(1, 1, 1, 1), vec4(0, 0, 1, 1), vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
		//
		////Texture* testAlbedo = Texture::LoadTextureFromPath("INVALID PATH");  // Bright red error texture
		//
		//newMaterial->SetTexture(testAlbedo, TEXTURE_ALBEDO);

		//// Add the material to our material list:
		//int materialIndex = AddMaterial(newMaterial);

		//// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		//mesh = Mesh::CreateCube();
		//mesh.MaterialIndex() = materialIndex;

		//currentScene->meshes.push_back(mesh);
		//meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address

		//// Assemble a list of all meshes held by a Renderable:
		//viewMeshes.clear();
		//viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
		//testRenderable = Renderable(viewMeshes);

		//// Construct a GameObject for the cube:
		//cubeObject = new GameObject("cubeObject2", testRenderable);

		//cubeObject->GetTransform()->SetPosition(vec3(3, 0, 0));
		//cubeObject->GetTransform()->SetEulerRotation(vec3(0.5f,0.5f,0.5f));


		//// Add cube object to scene:
		//currentScene->gameObjects.push_back(cubeObject);
		//gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		//// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		//currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());


		//




		//// Assemble a third cube:

		//// Create a material and shader:
		//newMaterial = new Material("testMaterial3", "thisShouldLoadTheErrorShader");


		////shaderIndex = GetShaderIndexFromShaderName(CoreEngine::GetCoreEngine()->GetConfig()->shader.errorShaderName); // Use the error shader
		////newMaterial = new Material("testMaterial3", shaderIndex);

		//// Create textures and assign them to the material:
		//testAlbedo = FindLoadTextureByPath("./another/invalid/path");
		//newMaterial->SetTexture(testAlbedo, TEXTURE_ALBEDO);

		//// Add the material to our material list:
		//materialIndex = AddMaterial(newMaterial);

		//// Construct a mesh and store it locally: (Normally, we'll do this when loading a .FBX)
		//mesh = Mesh::CreateCube();
		//mesh.MaterialIndex() = materialIndex;

		//currentScene->meshes.push_back(mesh);
		//meshIndex = (int)currentScene->meshes.size() - 1; // Store the index so we can pass the address

		//// Assemble a list of all meshes held by a Renderable:
		//viewMeshes.clear();
		//viewMeshes.push_back(&(currentScene->meshes.at(meshIndex))); // Store the address of our mesh to pass to our Renderable
		//testRenderable = Renderable(viewMeshes);

		//// Construct a GameObject for the cube:
		//cubeObject = new GameObject("cubeObject2", testRenderable);

		//cubeObject->GetTransform()->SetPosition(vec3(0, -5, -5));
		//cubeObject->GetTransform()->SetEulerRotation(vec3(-0.3f, 0.3f, -0.3f));


		//// Add cube object to scene:
		//currentScene->gameObjects.push_back(cubeObject);
		//gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		//// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		//currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());



			   


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

		LOG_ERROR("Could not find material \"" + materialName + "\"");
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
		
		LOG_ERROR("Material \"" + materialName + "\" does not exist... Creating a new material with default shader");

		// If we've made it this far, no material with the given name exists. Create it:
		Material* newMaterial = new Material(materialName, CoreEngine::GetCoreEngine()->GetConfig()->shader.defaultShaderName); // Assign the default shader

		return AddMaterial(newMaterial, false);
	}

	unsigned int SceneManager::AddMaterial(Material* newMaterial, bool checkForExisting) // checkForExisting = true by default
	{
		if (checkForExisting) // Check if a material with the same name exists, and return it if it does
		{
			int materialIndex = FindMaterialIndex(newMaterial->Name());
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
		// Pre-allocate our vector of vectors:
		materialMeshLists.clear();
		materialMeshLists.reserve(currentMaterialCount);
		for (unsigned int i = 0; i < currentMaterialCount; i++)
		{
			materialMeshLists.emplace_back(vector<Mesh*>());
			materialMeshLists.at(i).reserve(25);			// TO DO: Tune this value based on the actual number of meshes loaded?
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
					LOG("AssembleMaterialMeshLists() is skipping a mesh with out of bounds material index!");
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

		// If we've made it this far, try and load the texture
		return Texture::LoadTextureFromPath(texturePath);
	}


	// Traverse a scene, printing info:
	void SceneManager::BuildSceneObjects(aiNode* root, aiScene const* scene)
	{
		if (root == nullptr || scene == nullptr || root->mNumChildren <= 0)
		{
			return;
		}
		
		// Print this node's info:
		LOG("Name = " + string(root->mName.C_Str()) + ", # meshes = " + to_string(root->mNumMeshes));

		LOG(to_string(root->mTransformation.a1) + " " + to_string(root->mTransformation.a2) + " " + to_string(root->mTransformation.a3) + " " + to_string(root->mTransformation.a4));
		LOG(to_string(root->mTransformation.b1) + " " + to_string(root->mTransformation.b2) + " " + to_string(root->mTransformation.b3) + " " + to_string(root->mTransformation.b4));
		LOG(to_string(root->mTransformation.c1) + " " + to_string(root->mTransformation.c2) + " " + to_string(root->mTransformation.c3) + " " + to_string(root->mTransformation.c4));
		LOG(to_string(root->mTransformation.d1) + " " + to_string(root->mTransformation.d2) + " " + to_string(root->mTransformation.d3) + " " + to_string(root->mTransformation.d4));

		//root->


		//if (root->mParent == NULL)
			// Don't parent!


		for (unsigned int i = 0; i < root->mNumMeshes; i++)
		{
			/*if (root->mMeshes[i] && scene->mMeshes &&  scene-> ->mMeshes)
			{
				
			}*/
			LOG("Mesh #" + to_string(i) + " has index = " + to_string(root->mMeshes[i]));
		}

		// Recursively call all children:
		for (unsigned int i = 0; i < root->mNumChildren; i++)
		{
			BuildSceneObjects(root->mChildren[i], scene);
		}
	}
}


