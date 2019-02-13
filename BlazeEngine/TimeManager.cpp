#include "TimeManager.h"
#include "CoreEngine.h"
#include "SDL.h"

namespace BlazeEngine
{
	TimeManager::TimeManager() : EngineComponent()
	{
		coreEngine = nullptr;
		SetName("TimeManager");

		startTime = prevTime = currentTime = SDL_GetTicks();

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
		coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Time manager shutting down..." });
	}

	void TimeManager::Update()
	{
		prevTime = currentTime;
		currentTime = SDL_GetTicks();
		deltaTime = currentTime - prevTime;
	}
}

