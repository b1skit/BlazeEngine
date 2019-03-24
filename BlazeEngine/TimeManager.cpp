#include "TimeManager.h"
#include "CoreEngine.h"
#include "SDL.h"

namespace BlazeEngine
{
	// Static values:
	unsigned int TimeManager::startTime;
	unsigned int TimeManager::prevTime;
	unsigned int TimeManager::currentTime;
	double TimeManager::deltaTime;


	TimeManager::TimeManager() : EngineComponent("TimeManager")
	{
		startTime = prevTime = currentTime = SDL_GetTicks(); // TO DO: use the high res timer instead?
	}

	/*TimeManager::~TimeManager()
	{

	}*/

	TimeManager& TimeManager::Instance()
	{
		static TimeManager* instance = new TimeManager();
		return *instance;
	}

	void TimeManager::Startup()
	{

		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("TimeManager started!") });
	}

	void TimeManager::Shutdown()
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_LOG, this, new string("Time manager shutting down...") });
	}

	void TimeManager::Update()
	{
		prevTime = currentTime;
		currentTime = SDL_GetTicks();
		deltaTime = (double)(currentTime - prevTime);
	}
}

