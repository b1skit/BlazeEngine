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

// Initial allocation amounts
#define GAMEOBJECTS_RESERVATION_AMT			100		// TODO: Set these with meaningful values...
#define RENDERABLES_RESERVATION_AMT			100

#define CAMERA_TYPE_SHADOW_ARRAY_SIZE			10
#define CAMERA_TYPE_REFLECTION_ARRAY_SIZE		10


namespace BlazeEngine
{
	// Pre-declarations:
	class Camera;
	class aiTexture;
	struct Bounds;


	enum CAMERA_TYPE // Indexes for scene cameras used for different rendering roles. Rendered in the order defined here.
	{
		CAMERA_TYPE_SHADOW,
		CAMERA_TYPE_REFLECTION,
		CAMERA_TYPE_MAIN,			// The primary scene camera

		CAMERA_TYPE_COUNT			// Reserved: The number of camera types
	};


	// Container for all scene data:
	struct Scene
	{
		Scene();
		~Scene();

		// Meshes:
		//--------
		// Allocate an empty mesh array. Clears any existing mesh array
		void	InitMeshArray(int maxMeshes);

		int		AddMesh(Mesh* newMesh);
		void	DeleteMeshes();
		Mesh*	GetMesh(int meshIndex);

		// Cameras:
		//---------
		vector<Camera*> const&	GetCameras(CAMERA_TYPE cameraType);
		Camera*					GetMainCamera() { return sceneCameras[CAMERA_TYPE_MAIN].at(0); }
		void					RegisterCamera(CAMERA_TYPE cameraType, Camera* newCamera);
		void					ClearCameras();	// Destroys the scene's cameras (without deallocating the containing arrays)
		

		// Scene object containers:
		//-------------------------
		vector<GameObject*> gameObjects;	// Pointers to dynamically allocated GameObjects
		vector<Renderable*> renderables;	// Pointers to statically allocated renderables held by GameObjects

		
		// Lights:
		//--------
		/*vector<Light> forwardLights;*/
		/*vector<Light> deferredLights;*/

		vec3	ambientLight		= vec3(0.0f, 0.0f, 0.0f);
		Light	keyLight;

		Bounds& WorldSpaceSceneBounds()
		{
			return sceneWorldBounds;
		}

	private:
		Mesh** meshes = nullptr;
		int maxMeshes = 0;
		int meshCount = 0;

		vector<vector<Camera*>> sceneCameras;

		Bounds sceneWorldBounds;
	};


	// Scene Manager: Manages scenes
	class SceneManager : public EngineComponent, public EventListener
	{
	public:
		SceneManager(); // Reserve vector memory
		~SceneManager();

		// Singleton functionality:
		static SceneManager& Instance();
		SceneManager(SceneManager const&)	= delete; // Disallow copying of our Singleton
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

		inline unsigned int			NumMaterials()												{ return currentMaterialCount; }
		inline Material*			GetMaterial(unsigned int materialIndex)						{ return materials[materialIndex]; }
		Material*					GetMaterial(string materialName);
		
		inline vector<Mesh*> const*	GetRenderMeshes(unsigned int materialIndex)					{ return &materialMeshLists.at(materialIndex); } // TODO: Bounds checking?
		inline vector<Renderable*>*	GetRenderables()											{ return &currentScene->renderables;	}

		inline vec3 const&			GetAmbient()												{ return currentScene->ambientLight; }
		inline Light&				GetKeyLight()												{ return currentScene->keyLight; }
		
		//inline Camera**				GetCameras(CAMERA_TYPE cameraType, int& camCount)			{ return currentScene->GetCameras(cameraType, camCount); }
		inline vector<Camera*>		GetCameras(CAMERA_TYPE cameraType)							{ return currentScene->GetCameras(cameraType); }
		inline Camera*				GetMainCamera()							 					{ return currentScene->GetMainCamera(); }
		void						RegisterCamera(CAMERA_TYPE cameraType, Camera* newCamera)	{ currentScene->RegisterCamera(cameraType, newCamera); }

	protected:


	private:
		// Scene management:
		//------------------
		Scene* currentScene = nullptr;

		// Add a game object and register it with the various tracking lists
		void AddGameObject(GameObject* newGameObject);

		// Material management:
		//---------------------
		Material**			materials				= nullptr;
		const unsigned int	MAX_MATERIALS			= 100; // TODO: Replace this with something configurable/dynamic?
		unsigned int		currentMaterialCount	= 0;
		
		// Finds an existing material, or creates one using the default shader if none exists
		int GetMaterialIndex(string materialName);

		// Add a material to the material array
		// Warning: Material name MUST be unique if checkForExisting == false
		unsigned int AddMaterial(Material* newMaterial, bool checkForExisting = true);
		
		// Returns index of material with matching name, or -1 otherwise
		int GetMaterialIndexIfExists(string materialName); 

		// Helper function: Compiles vectors filled with meshes that use each material. Must be called once after all meshes have finished loading
		void AssembleMaterialMeshLists(); 
		vector<vector<Mesh*>> materialMeshLists;


		// Texture management:
		//--------------------
		Texture**			textures			= nullptr;
		const unsigned int	MAX_TEXTURES		= 100; // TODO: Replace this with something configurable/dynamic?
		unsigned int		currentTextureCount	= 0;
		
		// Find if a texture if it exists, or try and load it if it doesn't. Returns nullptr if file can't be loaded
		Texture*	FindLoadTextureByPath(string texturePath);


		// Scene setup/construction:
		//--------------------------

		// Assimp scene material and texture import helper:
		void		ImportMaterialsAndTexturesFromScene(aiScene const* scene, string sceneName);
		
		// Assimp scene texture import helper:
		Texture*	ExtractLoadTextureFromAiMaterial(aiTextureType textureType, aiMaterial* material, string sceneName);
		Texture*	FindTextureByNameInAiMaterial(string nameSubstring, aiMaterial* material, string sceneName);

		// Assimp scene material property helper:
		bool		ExtractPropertyFromAiMaterial(aiMaterial* material, vec3& targetProperty, char const* AI_MATKEY_TYPE, int unused0 = 0, int unused1 = 0);


		// Assimp scene geo import helper:
		void			ImportGameObjectGeometryFromScene(aiScene const* scene);

		// Scene geometry import helper: Create a GameObject transform hierarchy and return the GameObject parent. 
		// Note: Adds the GameObject to the currentScene's gameObjects
		GameObject*		FindCreateGameObjectParents(aiScene const* scene, aiNode* parent);

		// Scene geometry import helper : Combines seperated transform nodes found throughout the scene graph.
		// Finds and combines the FIRST instance of Translation, Scaling, Rotation matrices in the parenting hierarchy
		aiMatrix4x4		GetCombinedTransformFromHierarchy(aiScene const* scene, aiNode* parent, bool skipPostRotations = true);
		void			InitializeTransformValues(aiMatrix4x4 const& source, Transform* dest);	// Helper function: Copy transformation values from Assimp scene to BlazeEngine transform
		
		// Find a node with a name matching or containing name
		aiNode*			FindNodeContainingName(aiScene const* scene, string name);
		aiNode*			FindNodeRecursiveHelper(aiNode* rootNode, string name);	// Recursive helper function: Finds nodes containing name as a substring


		// Import light data from loaded scene
		void	ImportLightsFromScene(aiScene const* scene);

		// Import camera data from loaded scene
		void	ImportCamerasFromScene(aiScene const* scene = nullptr, bool clearCameras = false);
	};
}

