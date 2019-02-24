#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"

//class CoreEngine;
//using BlazeEngine::CoreEngine;

namespace BlazeEngine
{
	//SceneManager::~SceneManager()
	//{
	//}

	SceneManager& SceneManager::Instance()
	{
		static SceneManager* instance = new SceneManager();
		return *instance;
	}

	void SceneManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Scene manager started!" });
	}

	void SceneManager::Shutdown()
	{

		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Scene manager shutting down..." });
	}

	void SceneManager::Update()
	{
		// TO DO: Update every game object
	}

	void SceneManager::HandleEvent(EventInfo const * eventInfo)
	{

		return;
	}

	void SceneManager::LoadScene(string scenePath)
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "Could not load " + scenePath + ", as SceneManager.LoadScene() is not implemented. Using a debug hard coded path for now!" });

		// Flush any existing scene objects: (NOTE: Any objects that access these must be shut down first!)
		//gameObjects.clear();
		renderables.clear();
		meshes.clear();
		/*materials.clear();*/
		shaders.clear();
		/*lights.clear();
		mainCamera = Camera();*/

		// Load our .FBX:
		// ...

		// Create a new game object for every item in the .FBX:
		// ...

		// DEBUG: HARD CODE SOME OBJECTS TO WORK WITH:

		//GameObject testObject(this, "testObject");

	}
}


