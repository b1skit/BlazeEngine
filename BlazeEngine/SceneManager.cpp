#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#include "glm.hpp"
#include "gtc/constants.hpp"
using glm::pi;

#include "assimp/Importer.hpp"	// Importer interface
#include "assimp/postprocess.h"	// Post processing flags

#define STB_IMAGE_IMPLEMENTATION	// Only include this define ONCE in the project
#include "stb_image.h"				// STB image loader

#include <algorithm>
#include <string>
#include <stdio.h>



namespace BlazeEngine
{
	SceneManager::SceneManager() : EngineComponent("SceneManager")
	{
		stbi_set_flip_vertically_on_load(true);	// Set stb_image to flip the y-axis on loading to match OpenGL's style (So pixel (0,0) is in the bottom-left of the image)
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

		if (currentScene)
		{
			delete currentScene;
			currentScene = nullptr;
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

		if (textures)
		{
			for (unsigned int i = 0; i < currentTextureCount; i++)
			{
				if (textures[i])
				{
					delete textures[i];
				}
			}
			delete[] textures;
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
		PlayerObject* player = new PlayerObject(currentScene->mainCamera);
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

	
	void BlazeEngine::SceneManager::AddGameObject(GameObject* newGameObject)
	{
		currentScene->gameObjects.push_back(newGameObject);
		int gameObjectIndex = (int)currentScene->gameObjects.size() - 1;

		// Store a pointer to the GameObject's Renderable and add it to the list for the RenderManager
		currentScene->renderables.push_back(currentScene->gameObjects[gameObjectIndex]->GetRenderable());

		#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
			LOG("Added new GameObject to the scene: " + newGameObject->GetName());
		#endif	
	}


	void BlazeEngine::SceneManager::InitializeTransformValues(aiMatrix4x4 const& source, Transform* dest)
	{
		aiVector3D sourceScale, sourcePosition; // sourceRotation, 
		aiQuaternion sourceRotation;
		source.Decompose(sourceScale, sourceRotation, sourcePosition); // Extract the decomposed matrices

		// TO DO: Use Quaternions instead of euler angles...
		glm::quat sourceRotationAsGLMQuat(sourceRotation.w, sourceRotation.x, sourceRotation.y, sourceRotation.z);
		vec3 eulerRotation = glm::eulerAngles(sourceRotationAsGLMQuat);

		dest->SetPosition(vec3(sourcePosition.x, sourcePosition.y, sourcePosition.z));
		/*dest->SetEulerRotation(vec3(sourceRotation.x, sourceRotation.y, sourceRotation.z));*/
		dest->SetEulerRotation(vec3(eulerRotation.x, eulerRotation.y, eulerRotation.z));
		dest->SetScale(vec3(sourceScale.x, sourceScale.y, sourceScale.z));
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
					}

					LOG("Importing normal map texture (RGB) from material's bump slot");
					Texture* normalTexture = ExtractLoadTextureFromAiMaterial(aiTextureType_NORMALS, scene->mMaterials[currentMaterial], sceneName);
					if (normalTexture)
					{
						newMaterial->SetTexture(normalTexture, TEXTURE_NORMAL);
					}

					LOG("Importing emissive map texture (RGB) from material's incandescence slot");
					Texture* emissiveTexture = ExtractLoadTextureFromAiMaterial(aiTextureType_EMISSIVE, scene->mMaterials[currentMaterial], sceneName);
					if (emissiveTexture)
					{
						newMaterial->SetTexture(emissiveTexture, TEXTURE_EMISSIVE);
					}

					LOG("Importing roughness, metalic, & AO textures (R+G+B) from material's specular slot");
					Texture* RMAO = ExtractLoadTextureFromAiMaterial(aiTextureType_SPECULAR, scene->mMaterials[currentMaterial], sceneName);
					if (emissiveTexture)
					{
						newMaterial->SetTexture(emissiveTexture, TEXTURE_RMAO);
					}

					// TO DO: If no texture is detected, attempt to load the hard-coded value instead and store it in one of the material's generic properties?


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

					// TO DO: Extract more generic properties?
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
		int textureCount = material->GetTextureCount(textureType);
		if (textureCount <= 0)
		{
			if (textureType == aiTextureType_DIFFUSE)
			{
				aiColor4D color;
				if (AI_SUCCESS == material->Get("$clr.diffuse", 0, 0, color))
				{
					LOG_WARNING("Received material does not have the requested texture. Creating a 1x1 texture using the diffuse color");
					Texture* newTexture = new Texture(1, 1, true, vec4(color.r, color.g, color.b, color.a));
					return newTexture;
				}
			}
			else
			{
				LOG_WARNING("Received material does not have the requested texture");
				return nullptr;
			}
		}
		if (textureCount > 1)
		{
			LOG_WARNING("Received material has " + to_string(textureCount) + " of the requested texture type... Only the first will be extracted");
		}

		Texture* newTexture = nullptr;
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
			newTexture = FindLoadTextureByPath("errorPath"); // TO DO: Make "errorPath" a global/static string, so it can be reused anywhere?
		}
		
		return newTexture;
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
		// NOTE: There seems to be a bug in Assimp where nested groups sometimes get incorrect rotations... I haven't been able to lock it down 100%, but as a workaround just avoid nesting

		int numMeshes = scene->mNumMeshes;
		LOG("Found " + to_string(numMeshes) + " scene meshes");

		// Allocations:
		if (currentScene->meshes != nullptr)
		{
			delete currentScene->meshes;
		}
		currentScene->meshes	= new Mesh*[numMeshes];
		currentScene->numMeshes = numMeshes;
		for (int i = 0; i < numMeshes; i++)
		{
			currentScene->meshes[i] = nullptr;
		}
		
		currentScene->gameObjects.clear();
		currentScene->gameObjects.reserve(numMeshes); // Assuming that every GameObject will have at least 1 mesh...

		// Loop through each mesh in the scene graph:
		for (int currentMesh = 0; currentMesh < numMeshes; currentMesh++)
		{
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
				LOG_WARNING("Mesh is missing the following properties:");
				if (!scene->mMeshes[currentMesh]->HasPositions())				LOG_WARNING("\t - positions");
				if (!scene->mMeshes[currentMesh]->HasFaces())					LOG_WARNING("\t - faces");
				if (!scene->mMeshes[currentMesh]->HasNormals())					LOG_WARNING("\t - normals");
				if (!scene->mMeshes[currentMesh]->HasVertexColors(0))			LOG_WARNING("\t - vertex colors");
				if (!scene->mMeshes[currentMesh]->HasTextureCoords(0))			LOG_WARNING("\t - texture coordinates");
				if (!scene->mMeshes[currentMesh]->HasTangentsAndBitangents())	LOG_WARNING("\t - tangents & bitangents");
			}

			// See if we can find the corresponding node in the scene graph:
			string meshName = string(scene->mMeshes[currentMesh]->mName.C_Str());
			aiNode* currentNode = scene->mRootNode->FindNode(scene->mMeshes[currentMesh]->mName);

			if (currentNode)
			{
				// We've found a corresponding node in the scene graph. Create a mesh:
				int numVerts = scene->mMeshes[currentMesh]->mNumVertices;
				int numFaces = scene->mMeshes[currentMesh]->mNumFaces;
				int numUVs = scene->mMeshes[currentMesh]->mNumUVComponents[0]; // Just look at the first UV channel for now...
				int numUVChannels = scene->mMeshes[currentMesh]->GetNumUVChannels();
				int materialIndex = scene->mMeshes[currentMesh]->mMaterialIndex;

				#if defined(DEBUG_SCENEMANAGER_MESH_LOGGING)
					LOG("\nMesh #" + to_string(currentMesh) + " \"" + meshName + "\": " + to_string(numVerts) + " verts, " + to_string(numFaces) + " faces, " + to_string(numUVChannels) + " UV channels, " + to_string(numUVs) + " UV components in channel 0, using material #" + to_string(materialIndex));
				#endif

				Vertex* vertices = new Vertex[numVerts];

				// Fill the vertices array:
				for (int currentVert = 0; currentVert < numVerts; currentVert++)
				{
					// Default vertex values:
					vec3 position = vec3(0,0,0), normal = vec3(0, 0, 0), tangent = vec3(0, 0, 0), bitangent = vec3(0, 0, 0);
					vec4 color(0, 0, 0, 1);
					vec4 uv(0, 0, 0, 0);

					// Position:
					if (scene->mMeshes[currentMesh]->HasPositions())
					{
						position = vec3(scene->mMeshes[currentMesh]->mVertices[currentVert].x, scene->mMeshes[currentMesh]->mVertices[currentVert].y, scene->mMeshes[currentMesh]->mVertices[currentVert].z);
					}

					// Normal:
					if (scene->mMeshes[currentMesh]->HasFaces())
					{
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

					if (scene->mMeshes[currentMesh]->HasTangentsAndBitangents())
					{
						tangent = vec3(scene->mMeshes[currentMesh]->mTangents[currentVert].x, scene->mMeshes[currentMesh]->mTangents[currentVert].y, scene->mMeshes[currentMesh]->mTangents[currentVert].z);
						bitangent = vec3(scene->mMeshes[currentMesh]->mBitangents[currentVert].x, scene->mMeshes[currentMesh]->mBitangents[currentVert].y, scene->mMeshes[currentMesh]->mBitangents[currentVert].z);
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
			
				currentScene->meshes[currentMesh] = newMesh;				

				GameObject* gameObject = FindCreateGameObjectParents(scene, currentNode->mParent);

				// If the mesh doesn't belong to a group, create a GameObject to contain it:
				if (gameObject == nullptr)
				{
					gameObject = new GameObject(meshName + "_GAMEOBJECT"); // Add a postfix to remind us that we expect GameObjects to be grouped in our .FBX from Maya
						
					aiMatrix4x4 combinedTransform = GetCombinedTransformFromHierarchy(scene, currentNode);	// Pass the currentNode (instead of its parent), since this mesh doesn't belong to a group
					InitializeTransformValues(combinedTransform, gameObject->GetTransform());
						
					// Set the GameObject as the parent of the mesh:
					newMesh->GetTransform().SetParent(gameObject->GetTransform());

					gameObject->GetRenderable()->AddViewMeshAsChild(currentScene->meshes[currentMesh]);

					LOG_ERROR("Created a _GAMEOBJECT for mesh \"" + meshName + "\" that did not belong to a group! GameObjects should belong to groups in the source .FBX!");

					AddGameObject(gameObject);	// We need to manually add the game object
					continue; // We're done!
				}

				// We have a GameObject:
				aiMatrix4x4 combinedTransform = GetCombinedTransformFromHierarchy(scene, currentNode->mParent);
				combinedTransform = combinedTransform * currentNode->mTransformation; // Combine the parent and child transforms								

				InitializeTransformValues(combinedTransform, &currentScene->meshes[currentMesh]->GetTransform());  // Copy to our Mesh transform

				// Add the mesh to the GameObject's Renderable's viewMeshes:
				gameObject->GetRenderable()->AddViewMeshAsChild(currentScene->meshes[currentMesh]);
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
			newGameObject->GetTransform()->SetParent(nextParent->GetTransform());
		}
		
		#if defined(DEBUG_SCENEMANAGER_GAMEOBJECT_LOGGING)
			LOG("Returning newly created GameObject \"" + newGameObject->GetName() + "\"");
		#endif

		return newGameObject;
	}


	aiMatrix4x4 BlazeEngine::SceneManager::GetCombinedTransformFromHierarchy(aiScene const* scene, aiNode* parent)
	{
		if (scene == nullptr || parent == nullptr)
		{
			LOG_ERROR("SceneManager.GetCombinedTransformFromHierarchy() received a null pointer!");
			return aiMatrix4x4();
		}

		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
			LOG("Received parent \"" + (parent == nullptr ? "nullptr" : string(parent->mName.C_Str())) + "\". Combining imported transformations from scene graph:");
		#endif

		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
			LOG("Spewing transform hierarchy:");
			aiNode* debug = parent;
			while (debug != nullptr)
			{
				LOG("-> " + string(debug->mName.C_Str()));

				LOG("\t\t" + to_string(debug->mTransformation.a1) + " " + to_string(debug->mTransformation.b1) + " " + to_string(debug->mTransformation.c1) + " " + to_string(debug->mTransformation.d1));
				LOG("\t\t" + to_string(debug->mTransformation.a2) + " " + to_string(debug->mTransformation.b2) + " " + to_string(debug->mTransformation.c2) + " " + to_string(debug->mTransformation.d2));
				LOG("\t\t" + to_string(debug->mTransformation.a3) + " " + to_string(debug->mTransformation.b3) + " " + to_string(debug->mTransformation.c3) + " " + to_string(debug->mTransformation.d3));
				LOG("\t\t" + to_string(debug->mTransformation.a4) + " " + to_string(debug->mTransformation.b4) + " " + to_string(debug->mTransformation.c4) + " " + to_string(debug->mTransformation.d4));

				debug = debug->mParent;
			}
			LOG("End of transform hierarchy!");
		#endif

		string parentName = string(parent->mName.C_Str());
		if (parentName.find("$AssimpFbx$") == string::npos)
		{
			#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
				LOG("\tParent \"" + parentName + "\" is not a transformation, returning identity instead of traversing up to \"" + (parent->mParent ? string(parent->mParent->mName.C_Str()) : "null parent") + "\"");
			#endif
			return aiMatrix4x4(); // Later, this is multiplied by the transform of the current node
		}

		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
			LOG("Starting with transform of \"" + parentName + "\":");
		#endif

		aiMatrix4x4 combinedTransform;
		aiNode* current = parent;
		while (current != nullptr && current != scene->mRootNode)
		{
			string currentName = string(current->mName.C_Str());

			if (currentName.find("_Post") != string::npos) // HACK: Seems if we skip "_PostRotation" nodes, the directional light orientation will be correct
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



		//aiMatrix4x4 translation;
		//aiMatrix4x4 scaling;
		//aiMatrix4x4 rotation;

		//aiNode* current = parent;
		//bool foundTranslation = false, foundScaling = false, foundRotation = false;
		//while (current != nullptr && current != scene->mRootNode)
		//{
		//	string currentName = string(current->mName.C_Str());

		//	// Assimp (seems to) build transformations in the order Scaling, Rotation, Translation as we walk up the tree...
		//	if (currentName.find("$AssimpFbx$") != string::npos)
		//	{
		//		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
		//				LOG("\tCurrent node is \"" + currentName + "\":");
		//		#endif


		//		if (!foundTranslation && currentName.find("Translation") != string::npos && currentName.find("Pivot") == string::npos)
		//		{
		//			translation = current->mTransformation;
		//			foundTranslation = true;
		//			#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
		//				LOG("\t\t\"" + currentName + "\" is a translation node!");
		//			#endif
		//		}
		//		else if (!foundScaling && currentName.find("Scaling") != string::npos && currentName.find("Pivot") == string::npos)
		//		{
		//			scaling = current->mTransformation;
		//			foundScaling = true;
		//			#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
		//				LOG("\t\t\"" + currentName + "\" is a scaling node!");
		//			#endif
		//		}
		//		else if (!foundRotation && currentName.find("_Rotation") != string::npos && currentName.find("Pivot") == string::npos) // We check for "_Rotation" so we can skip "PostRotation"
		//		{
		//			rotation = current->mTransformation;
		//			foundRotation = true;
		//			#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
		//				LOG("\t\t\"" + currentName + "\" is a rotation node!");
		//			#endif
		//		}
		//		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
		//			else
		//			{
		//				LOG("\t\tSkipping \"" + currentName + "\"...");
		//			}
		//		#endif
		//	}
		//	else 
		//	{
		//		#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
		//			LOG("Found a non-transformation node, stopping search");
		//		#endif
		//		break; // If we've found a non-transformation node, we need to stop searching
		//	}

		//	if (foundTranslation && foundScaling && foundRotation)
		//	{
		//		break;
		//	}

		//	current = current->mParent;
		//}
		//#if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
		//	LOG("Finished combining transformations");
		//#endif
		//return translation * scaling * rotation;
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
		// TO DO: Add logging: #if defined(DEBUG_SCENEMANAGER_TRANSFORM_LOGGING)
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
					}
					
					vec3 lightColor(scene->mLights[i]->mColorDiffuse.r, scene->mLights[i]->mColorDiffuse.g, scene->mLights[i]->mColorDiffuse.b);

					currentScene->keyLight = Light
					(
						lightName, 
						LIGHT_DIRECTIONAL, 
						lightColor
					);

					InitializeTransformValues(lightTransform, &currentScene->keyLight.GetTransform());

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


	void BlazeEngine::SceneManager::ImportCamerasFromScene(aiScene const* scene /*= nullptr*/)
	{
		// Delete the existing camera, if necessary:
		if (currentScene->mainCamera != nullptr)
		{
			delete currentScene->mainCamera;
			currentScene->mainCamera = nullptr;
		}

		if (scene == nullptr) // Signal to create a default camera at the origin
		{
			LOG("Creating a default camera");
			currentScene->mainCamera = new Camera("defaultCamera");
			currentScene->mainCamera->Initialize(
				vec3(0, 0, 0),
				(float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes / (float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
				CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultFieldOfView,
				CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultNear,
				CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultFar
			);
			return;
		}

		// If we made it this far, we've imported a camera from the scene:
		string cameraName = string(scene->mCameras[0]->mName.C_Str());
		currentScene->mainCamera = new Camera(cameraName); // TO DO: Should main camera ALWAYS be camera 0 ??

		int numCameras = scene->mNumCameras;
		if (numCameras > 1)
		{
			LOG_ERROR("Found " + to_string(numCameras) + " cameras in the scene. Currently, only 1 camera is supported. Setting the FIRST camera received camera as the main camera.");
		}
		else
		{
			LOG("Found " + to_string(numCameras) + " scene camera");
		}


		// Currently, camera transforms seem to be broken in Assimp...
		// DEBUG: Set a default camera at the origin, for now
		LOG_ERROR("Assimp camera import is broken... Creating a default camera at the origin");
		currentScene->mainCamera->Initialize
		(
			vec3(0, 0, 0),
			(float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes / (float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
			CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultFieldOfView,
			CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultNear,
			CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultFar
		);
		// END DEBUG
	

		// Note: In the current version of Assimp, the mLookAt, mUp vectors seem to just be the world forward, up vectors, and mTransformation is the identity...
		// TO DO: TEST: Are these possibly related to specific maya camera creation options?
		// + Importing cameras vacing towards Z+ results in a flipped camera?

		// TO DO: Add ALL cameras (look for a name that contains "main" to use as the main camera, or use the 1st camera otherwise)

		//aiNode* camNode = scene->mRootNode->FindNode(scene->mCameras[0]->mName);
		//if (camNode)
		//{			
		//	aiMatrix4x4 camTransform;
		//	aiNode* current = camNode->mParent;
		//	while (current != nullptr && current != scene->mRootNode)
		//	{
		//		string currentName = string(current->mName.C_Str());

		//		if (currentName.find("_Post") != string::npos)
		//		{
		//			#if defined(DEBUG_SCENEMANAGER_CAMERA_LOGGING)
		//				LOG("Skipping camera transform node \"" + currentName + "\"");
		//			#endif
		//			current = current->mParent;
		//			continue;
		//		}

		//		if (currentName.find("$AssimpFbx$") != string::npos)
		//		{
		//			#if defined(DEBUG_SCENEMANAGER_CAMERA_LOGGING)
		//				LOG("Found camera transform node \"" + currentName + "\"");
		//			#endif
		//			//camTransform = camTransform * current->mTransformation;
		//			camTransform = current->mTransformation * camTransform;
		//		}
		//		else
		//		{
		//			#if defined(DEBUG_SCENEMANAGER_CAMERA_LOGGING)
		//				LOG("Node \"" + currentName + "\" is not a camera transform. Stopping!");
		//			#endif
		//			break;
		//		}
		//		current = current->mParent;
		//	}

		//	#if defined(DEBUG_SCENEMANAGER_CAMERA_LOGGING)
		//		LOG("Combining transforms with camera's local transform node \"" + string(camNode->mName.C_Str()) + "\"");
		//	#endif
		//	//camTransform = camTransform * camNode->mTransformation;
		//	camTransform = camNode->mTransformation * camTransform;
		//	 
		//	InitializeTransformValues(camTransform, currentScene->mainCamera->GetTransform());
		//}

		vec3 camPosition = currentScene->mainCamera->GetTransform()->Position();
		vec3 camRotation = currentScene->mainCamera->GetTransform()->GetEulerRotation();

		#if defined(DEBUG_SCENEMANAGER_CAMERA_LOGGING)
			LOG("Camera is located at " + to_string(camPosition.x) + " " + to_string(camPosition.y) + " " + to_string(camPosition.z) + ". Near = " + to_string(scene->mCameras[0]->mClipPlaneNear) + ", " + "far = " + to_string(scene->mCameras[0]->mClipPlaneFar) );
			LOG("Camera rotation is " + to_string(camRotation.x) + " " + to_string(camRotation.y) + " " + to_string(camRotation.z) + " (radians)");
			LOG("Camera rotation is " + to_string(camRotation.x * (180.0f / glm::pi<float>())) + " " + to_string(camRotation.y * (180.0f / glm::pi<float>())) + " " + to_string(camRotation.z * (180.0f / glm::pi<float>())) + " (degrees)");
		#endif

		LOG_ERROR("Camera field of view is NOT currently loaded from the source file. A hard-coded default value is used for now...");

		currentScene->mainCamera->Initialize(
			(float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes / (float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes,
			CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultFieldOfView, //scene->mCameras[0]->mHorizontalFOV; // TO DO: Implement this (Needs to be converted to a vertical FOV???)
			scene->mCameras[0]->mClipPlaneNear,
			scene->mCameras[0]->mClipPlaneFar
		);
	}
}


