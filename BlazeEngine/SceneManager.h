#pragma once

#include "EngineComponent.h"
#include "EventListener.h"
#include "SceneObject.h"
#include "GameObject.h"
#include "Renderable.h"
//#include "Material.h"
#include "Shader.h"
//#include "Light.h"
//#include "Camera.h"

//class GameObject;
//class SceneObject;

#include <vector>

using std::vector;

namespace BlazeEngine
{
	class SceneManager : public EngineComponent, public EventListener
	{
	public:
		SceneManager() : EngineComponent("SceneManager") {}

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

		inline vector<Renderable const*> const* GetRenderables()
		{
			return &renderables;
		}
		/*inline vector<Light> const* GetLights();
		inline Camera const* GetCamera();*/

	protected:


	private:
		// The scene manager holds and provides access to all of the scene objects needed by the rest of the engine:
		vector<GameObject> gameObjects;
		vector<Renderable const*> renderables; // Point directly to the renderables of our gameObjects
		vector<Mesh> meshes;
		vector<Material> materials;

		/*vector<Light> lights;*/
		/*Camera mainCamera;*/

	};

}

