#pragma once

#include "EngineComponent.h"
#include "EventListener.h"

#include "assimp/scene.h"		// Output data structure

#include "glm.hpp"

#include <vector>

using std::vector;
using glm::vec4;


namespace BlazeEngine
{
	// Pre-declarations:
	class Camera;
	class aiTexture;
	class SceneObject;
	class GameObject;
	class Material;
	class Texture;
	class Renderable;
	class Light;
	class Transform;
	class Mesh;
	class Skybox;
	struct Bounds;
	struct Scene;
	enum CAMERA_TYPE;


	// Scene Manager: Manages scenes
	class SceneManager : public EngineComponent, public EventListener
	{
	public:
		SceneManager(); // Reserve vector memory

		// Singleton functionality:
		static SceneManager& Instance();
		SceneManager(SceneManager const&)	= delete; // Disallow copying of our Singleton
		void operator=(SceneManager const&) = delete;

		// EngineComponent interface:
		void Startup();
		void Shutdown();
		void Update();
		void Destroy() {}	// Do nothing, for now...

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Member functions:
		//------------------
		// Load a scene.
		// sceneName == the root folder name within the ./Scenes/ directory. Must contain an .fbx file with the same name.
		bool LoadScene(string sceneName);

		inline unsigned int				NumMaterials()												{ return currentMaterialCount; }
		Material*						GetMaterial(unsigned int materialIndex);
		Material*						GetMaterial(string materialName);
		
		vector<Mesh*> const*			GetRenderMeshes(int materialIndex = -1);					// If materialIndex is out of bounds, returns ALL meshes
		vector<Renderable*>*			GetRenderables();

		Light* const&					GetAmbientLight();
		Light*							GetKeyLight();
		
		vector<Camera*> const&			GetCameras(CAMERA_TYPE cameraType);
		Camera*							GetMainCamera();
		void							RegisterCamera(CAMERA_TYPE cameraType, Camera* newCamera);;

		int								AddTexture(Texture*& newTexture); // Returns index of inserted texture. Updates pointer if duplicate texture exists

		vector<Light*> const&			GetDeferredLights();

		Skybox*							GetSkybox();

		string							GetCurrentSceneName() const;

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
		int					GetMaterialIndex(string materialName);

		// Add a material to the material array
		// Warning: Material name MUST be unique if checkForExisting == false
		unsigned int		AddMaterial(Material* newMaterial, bool checkForExisting = true);
		
		// Returns index of material with matching name, or -1 otherwise
		int					GetMaterialIndexIfExists(string materialName); 

		// Helper function: Compiles vectors filled with meshes that use each material. Must be called once after all meshes have finished loading
		void				AssembleMaterialMeshLists(); 
		vector<vector<Mesh*>> materialMeshLists;


		// Texture management:
		//--------------------
		Texture**			textures			= nullptr;	// TODO: Replace this with vector<Texture*>
		const unsigned int	MAX_TEXTURES		= 100;
		unsigned int		currentTextureCount	= 0;
		
		// Find if a texture if it exists, or try and load it if it doesn't. Returns nullptr if file isn't/can't be loaded
		Texture*		FindLoadTextureByPath(string texturePath, bool loadIfNotFound = true);


		// Scene setup/construction:
		//--------------------------

		// Assimp scene material and texture import helper:
		void			ImportMaterialsAndTexturesFromScene(aiScene const* scene, string sceneName);

		// Import and configure scene skybox:
		void			ImportSky(string sceneName);
		
		// Assimp scene texture import helper:
		Texture*		ExtractLoadTextureFromAiMaterial(aiTextureType textureType, aiMaterial* material, string sceneName);
		Texture*		FindTextureByNameInAiMaterial(string nameSubstring, aiMaterial* material, string sceneName);

		// Assimp scene material property helper:
		bool			ExtractPropertyFromAiMaterial(aiMaterial* material, vec4& targetProperty, char const* AI_MATKEY_TYPE, int unused0 = 0, int unused1 = 0); // NOTE: unused0/unused1 are required to match #defined macros


		// Assimp scene geo import helper:
		void			ImportGameObjectGeometryFromScene(aiScene const* scene);

		// Scene geometry import helper: Create a GameObject transform hierarchy and return the GameObject parent. 
		// Note: Adds the GameObject to the currentScene's gameObjects
		GameObject*		FindCreateGameObjectParents(aiScene const* scene, aiNode* parent);

		// Scene geometry import helper : Combines seperated transform nodes found throughout the scene graph.
		// Finds and combines the FIRST instance of Translation, Scaling, Rotation matrices in the parenting hierarchy
		aiMatrix4x4		GetCombinedTransformFromHierarchy(aiScene const* scene, aiNode* parent, bool skipPostRotations = true);
		void			InitializeTransformValues(aiMatrix4x4 const& source, Transform* dest);	// Helper function: Copy transformation values from Assimp scene to BlazeEngine transform

		// Light import helper: Initializes a BlazeEngine Light's transform from an assimp scene. Calls InitializeTransformValues()
		void			InitializeLightTransformValues(aiScene const* scene, string lightName, Transform* targetLightTransform);
		
		// Find a node with a name matching or containing name
		aiNode*			FindNodeContainingName(aiScene const* scene, string name);
		aiNode*			FindNodeRecursiveHelper(aiNode* rootNode, string name);	// Recursive helper function: Finds nodes containing name as a substring


		// Import light data from loaded scene
		void			ImportLightsFromScene(aiScene const* scene);

		// Import camera data from loaded scene
		void			ImportCamerasFromScene(aiScene const* scene = nullptr, bool clearCameras = false);
	};
}

