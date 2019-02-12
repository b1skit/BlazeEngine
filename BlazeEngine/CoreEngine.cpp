#include <iostream>
#include "CoreEngine.h"
#include "EventManager.h"

#include "SDL.h"

#include <string>



namespace BlazeEngine
{
	CoreEngine::CoreEngine() : BlazeObject()
	{
		SetName("CoreEngine");

		// Initialize SDL:
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			exit(-1);
		}
	};

	void CoreEngine::Startup()
	{
		// Initialize BlazeEngine:
		BlazeEventManager = &EventManager::Instance();
		BlazeEventManager->Startup(this);
		
		BlazeLogManager = &LogManager::Instance();
		BlazeLogManager->Startup(this);

		BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "CoreEngine started!" }, true);

		BlazeTimeManager = &TimeManager::Instance();
		BlazeTimeManager->Startup(this);

		isRunning = true;

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "CoreEngine beginning main game loop!" });

		int debugCount = 0;

		double timeCount = 0.0;

		while (isRunning)
		{
			BlazeTimeManager->Update();

			BlazeEventManager->Update();

			timeCount += BlazeTimeManager->GetDeltaTimeSeconds();
			if (timeCount >= 1)
			{
				BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Tick..."} );
				timeCount -= 1.0;
			}
			

			BlazeLogManager->Update();

			if (debugCount == 5000000)// DEBUG
				isRunning = false;
			else
				debugCount++;
		}
	}

	void CoreEngine::Shutdown()
	{
		BlazeEventManager->Notify(EventInfo{EVENT_LOG, this, "CoreEngine shutting down..."});


		BlazeEventManager->Shutdown();

		return;
	}
}