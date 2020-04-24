#include "TimeManager.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#include "SDL.h"

namespace BlazeEngine
{
	// Static values:
	unsigned int TimeManager::startTime;
	unsigned int TimeManager::prevTime;
	unsigned int TimeManager::currentTime;
	unsigned int TimeManager::deltaTime;


	TimeManager::TimeManager() : EngineComponent("TimeManager")
	{
		startTime = prevTime = currentTime = SDL_GetTicks();
	}


	TimeManager& TimeManager::Instance()
	{
		static TimeManager* instance = new TimeManager();
		return *instance;
	}


	void TimeManager::Startup()
	{
		LOG("TimeManager starting...");
	}


	void TimeManager::Shutdown()
	{
		LOG("Time manager shutting down...");
	}


	void TimeManager::Update()
	{
		prevTime	= currentTime;
		currentTime = SDL_GetTicks();
		deltaTime	= (currentTime - prevTime);
	}
}

