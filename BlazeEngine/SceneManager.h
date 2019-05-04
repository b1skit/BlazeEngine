#pragma once

#include "EngineComponent.h"
#include "EventListener.h"
#include "SceneObject.h"
#include "GameObject.h"
#include "Renderable.h"
#include "Material.h"
#include "Shader.h"
#include "Light.h"
#include "PlayerObject.h"

#include "assimp/scene.h"		// Output data structure

#include <vector>

using std::vector;



namespace BlazeEngine
{
	// Pre-declarations:
	class Camera;
	class aiTexture;


	// Container for all scene data:
	struct Scene
	{
		Scene()
		{
			// TODO: Set these with meaningful values...
			gameObjects.reserve(100);
			renderables.reserve(100);
			meshes.reserve(100);

			/*forwardLights.reserve(100);*/
			/*deferredLights.reserve(100);*/
		}

		~Scene()
		{
			for (int i = 0; i < (int)meshes.size(); i++)
			{
				meshes.at(i).DestroyMesh();
			}

			for (int i = 0; i < gameObjects.size(); i++)
			{
				if (gameObjects.at(i))
				{
					delete gameObjects.at(i);
					gameObjects.at(i) = nullptr;
				}
			}

			if (mainCamera != nullptr)
			{
				delete mainCamera;
			}
		}


		// Cameras:
		Camera* mainCamera		= nullptr;	// Main camera: Currently points towards player object cam
		/*vector<Camera> sceneCameras;*/	// Various render cams

		// Meshes and scene objects:
		vector<GameObject*> gameObjects;
		vector<Renderable*> renderables; // Pointers to statically allocated renderables held by GameObjects
		vector<Mesh> meshes;

		// Scene Lights: A scene can have ???
		/*vector<Light> forwardLights;*/
		/*vector<Light> deferredLights;*/

		vec3 ambientLight		= vec3(1.0f, 1.0f, 1.0f);
		float ambientIntensity	= 1.0f;

		Light keyLight;
	};


	// Scene Manager: Manages scenes
	class SceneManager : public EngineComponent, public EventListener
	{
	public:
		SceneManager(); // Reserve vector memory
		~SceneManager();

		// Singleton functionality:
		static SceneManager& Instance();
		SceneManager(SceneManager const&) = delete; // Disallow copying of our Singleton
		void operator=(SceneManager const&) = delete;

		// EngineComponent interface:
		void Startup();
		void Shutdown();
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Member functions:
		//------------------
		// Load a scene.
		// sceneName == the root folder name within the ./Scenes/ directory. Must contain an .fbx file with the same name.
		void LoadScene(string sceneName);

		inline unsigned int NumMaterials()										{ return currentMaterialCount; }
		inline Material* GetMaterial(unsigned int materialIndex)				{ return materials[materialIndex]; }
		
		Material* GetMaterial(string materialName);
		
		inline vector<Mesh*> const* GetRenderMeshes(unsigned int materialIndex) { return &materialMeshLists.at(materialIndex); } // TO DO: BOunds checking?

		inline vector<Renderable*>* GetRenderables()							{ return &currentScene->renderables;	}

		inline vec3 const& GetAmbient()											{ return currentScene->ambientLight; }
		inline Light& GetKeyLight()												{ return currentScene->keyLight; }
		/*inline vector<Light> const& GetForwardLights() { return forwardLights; }*/
		
		inline Camera* MainCamera()												{ return currentScene->mainCamera; }

	protected:


	private:
		// Scene management:
		//------------------
		Scene* currentScene = nullptr;

		// Add a game object and register it with the various tracking lists
		void AddGameObject(GameObject* newGameObject);

		// Helper function: Copy transformation values from Assimp scene to BlazeEngine transform
		void InitializeTransformValues(aiMatrix4x4 const& source, Transform* dest);

		// Material management:
		//---------------------
		const unsigned int MAX_MATERIALS	= 100; // TO DO: Replace this with something configurable/dynamic?
		unsigned int currentMaterialCount	= 0;
		Material** materials				= nullptr;

		// Finds an existing material, or creates one using the default shader if none exists
		int GetMaterialIndex(string materialName);

		// Add a material to the material array
		// Warning: Material name MUST be unique if checkForExisting == false
		unsigned int AddMaterial(Material* newMaterial, bool checkForExisting = true);
		
		// Returns index of material with matching name, or -1 otherwise
		int FindMaterialIndex(string materialName); 

		// Helper function: Compiles vectors filled with meshes that use each material. Must be called once after all meshes have finished loading
		void AssembleMaterialMeshLists(); 
		vector<vector<Mesh*>> materialMeshLists;


		// Texture management:
		//--------------------
		const unsigned int MAX_TEXTURES		= 100; // TO DO: Replace this with something configurable/dynamic?
		Texture** textures					= nullptr;
		unsigned int currentTextureCount	= 0;
		
		// Find if a texture if it exists, or try and load it if it doesn't. Returns nullptr if file can't be loaded
		Texture* FindLoadTextureByPath(string texturePath);


		// Scene setup/construction:
		//--------------------------

		// Assimp scene material and texture import helper:
		void ImportMaterialsAndTexturesFromScene(aiScene const* scene, string sceneName);
		
		// Assimp scene texture import helper:
		Texture* ExtractLoadTextureFromAiMaterial(aiTextureType textureType, aiMaterial* material, string sceneName);

		// Assimp scene material property helper:
		bool ExtractPropertyFromAiMaterial(aiMaterial* material, vec3& targetProperty, char const* AI_MATKEY_TYPE, int unused0 = 0, int unused1 = 0);

		// Assimp scene geo import helper:
		void ImportGameObjectGeometryFromScene(aiScene const* scene);


		// Scene geometry import helper: Create a GameObject transform hierarchy and return the GameObject parent. 
		// Note: Adds the GameObject to the currentScene's gameObjects
		GameObject* FindCreateGameObjectParents(aiScene const* scene, aiNode* parent);

		// Scene geometry import helper : Combines seperated transform nodes found throughout the scene graph.
		// Finds and combines the FIRST instance of Translation, Scaling, Rotation matrices in the parenting hierarchy
		aiMatrix4x4 GetCombinedTransformFromHierarchy(aiScene const* scene, aiNode* parent);
		
		// Find a node with a name matching or containing name
		aiNode* FindNodeContainingName(aiScene const* scene, string name);

		// Recursive helper function: Finds nodes containing name as a substring
		aiNode* FindNodeRecursiveHelper(aiNode* rootNode, string name);

		//
		void ImportLightsFromScene(aiScene const* scene);

		//
		void ImportCamerasFromScene(aiScene const* scene = nullptr);

	};
}

