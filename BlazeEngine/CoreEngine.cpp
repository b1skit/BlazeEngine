#include <iostream>
#include "CoreEngine.h"

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
		BlazeEventManager->Subscribe(EVENT_ENGINE_QUIT, this);

		BlazeTimeManager = &TimeManager::Instance();
		BlazeTimeManager->Startup(this);

		BlazeInputManager = &InputManager::Instance();
		BlazeInputManager->Startup(this);

		BlazeRenderManager = &RenderManager::Instance();
		BlazeRenderManager->Startup(this);

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
			
			BlazeRenderManager->Render(elapsed/FIXED_TIMESTEP); // Render precise current position
			
			
		}
	}

	void CoreEngine::Stop()
	{
		isRunning = false;
	}

	void CoreEngine::Shutdown()
	{
		BlazeEventManager->Notify(EventInfo{EVENT_LOG, this, "CoreEngine shutting down..."});

		

		BlazeEventManager->Shutdown();
		BlazeTimeManager->Shutdown();		
		BlazeInputManager->Shutdown();
		BlazeRenderManager->Shutdown();

		BlazeEventManager->Update();

		return;
	}

	int CoreEngine::HandleEvent(EventInfo eventInfo)
	{
		switch (eventInfo.type)
		{
		case EVENT_ENGINE_QUIT:
			Stop();
			break;

		default:
			cout << "ERROR: Default event generated in CoreEngine!\n";
			break;
		}

		return 0;
	}
}