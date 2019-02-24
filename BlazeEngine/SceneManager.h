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
		/*~SceneManager();*/

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

		inline vector<Renderable> const* GetRenderables()
		{
			return &renderables;
		}
		/*inline vector<Light> const* GetLights();
		inline Camera const* GetCamera();*/

	protected:


	private:
		//vector<GameObject> gameObjects;
		vector<Renderable> renderables;
		vector<Mesh> meshes;
		/*vector<Material> materials;*/
		vector<Shader> shaders;
		/*vector<Light> lights;*/
		/*Camera mainCamera;*/

	};

}

