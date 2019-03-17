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


#include <vector>

using std::vector;



namespace BlazeEngine
{
	// Pre-declarations:
	class Camera;


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

		// Cameras:
		Camera* mainCamera; // Main camera: Currently points towards player object cam
		/*vector<Camera> sceneCameras;*/ // Various render cams

		// Meshes and scene objects:
		vector<GameObject*> gameObjects;
		vector<Renderable const*> renderables; // Pointers to statically allocated renderables held by GameObjects
		vector<Mesh> meshes;

		// Scene Lights: A scene can have ???
		/*vector<Light> forwardLights;*/
		/*vector<Light> deferredLights;*/

		vec3 ambientLight = vec3(1.0f, 1.0f, 1.0f);
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
		void Startup(CoreEngine * coreEngine);
		void Shutdown();
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Member functions:
		void LoadScene(string scenePath);

		inline unsigned int NumMaterials() { return currentMaterialCount; }
		inline Material* GetMaterial(unsigned int materialIndex) { return materials[materialIndex]; }
		inline vector<Mesh*> const* GetRenderMeshes(unsigned int materialIndex) { return &materialMeshLists.at(materialIndex); } // TO DO: BOunds checking?

		inline vector<Renderable const*> const* GetRenderables() { return &currentScene->renderables;	}
		inline vector<Shader>* GetShaders() { return &shaders; } // SHOULD THIS RETURN CONST ?????
		
		inline unsigned int GetShaderIndex(unsigned int materialIndex) { return materials[materialIndex]->GetShaderIndex(); } // TO DO: Bounds checking?

		/*inline vector<Light> const& GetForwardLights() { return forwardLights; }*/
		inline vec3 const& GetAmbient() { return currentScene->ambientLight; }
		
		inline Camera* MainCamera() { return currentScene->mainCamera; }

	protected:


	private:
		// Material management:
		const unsigned int MAX_MATERIALS = 100; // TO DO: Replace this with something dynamic?
		unsigned int currentMaterialCount = 0;
		Material** materials = nullptr;
		unsigned int AddMaterial(Material* newMaterial);

		vector<vector<Mesh*>> materialMeshLists;
		void AssembleMaterialMeshLists();

		
		vector<Shader> shaders; // TO DO: Replace with a dynamic array

		Scene* currentScene = nullptr;


		

		// Shader functions:
		unsigned int GetShaderIndex(string shaderName);
		int CreateShader(string shaderName);
		string LoadShaderFile(const string& filepath);
		GLuint CreateGLShaderObject(const string& text, GLenum shaderType);
		bool CheckShaderError(GLuint shader, GLuint flag, bool isProgram);
	};
}

