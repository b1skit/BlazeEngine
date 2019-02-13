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

		BlazeInputManager = &InputManager::Instance();
		BlazeInputManager->Startup(this);

		isRunning = true;

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "CoreEngine beginning main game loop!" });

		double elapsed = 0.0;	

		while (isRunning)
		{
			BlazeInputManager->Update(); // Process input

			BlazeTimeManager->Update();
			elapsed += BlazeTimeManager->GetDeltaTimeMs();

			while (elapsed >= FIXED_TIMESTEP)
			{
				/*BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Update loop called: " + std::to_string(elapsed)});*/

				// Update components:
				BlazeEventManager->Update();
				BlazeLogManager->Update();


				// Update the time for the next iteration:
				BlazeTimeManager->Update();
				elapsed -= FIXED_TIMESTEP;
			}
			
			// TO DO: BlazeRenderManager->Render(elapsed/FIXED_TIMESTEP); // Render precise current position
			/*BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Pretending to render at ~60fps..." });*/
			SDL_Delay((unsigned int)(1000.0 / 60.0));
		}
	}

	void CoreEngine::Shutdown()
	{
		BlazeEventManager->Notify(EventInfo{EVENT_LOG, this, "CoreEngine shutting down..."});


		BlazeEventManager->Shutdown();

		return;
	}
}