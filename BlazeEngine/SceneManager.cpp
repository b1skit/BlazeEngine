#include "SceneManager.h"
#include "EventManager.h"
#include "CoreEngine.h"

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

	}

	void SceneManager::HandleEvent(EventInfo const * eventInfo)
	{

		return;
	}

	void SceneManager::UpdateSceneObjects()
	{

	}


}


