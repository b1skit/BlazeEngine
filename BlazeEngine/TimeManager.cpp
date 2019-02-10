#include "TimeManager.h"
#include "CoreEngine.h"



namespace BlazeEngine
{
	TimeManager::TimeManager() : EngineComponent()
	{
		coreEngine = nullptr;
		SetName("TimeManager");

		startTime = time(nullptr);

	}


	/*TimeManager::~TimeManager()
	{

	}*/

	TimeManager& TimeManager::Instance()
	{
		static TimeManager* instance = new TimeManager();
		return *instance;
	}

	void TimeManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "TimeManager started!" });
	}

	void TimeManager::Shutdown()
	{

	}

	void TimeManager::Update()
	{

	}
}

