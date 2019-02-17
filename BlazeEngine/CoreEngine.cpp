#include <iostream>
#include "CoreEngine.h"

#include "SDL.h"

#include <string>



namespace BlazeEngine
{
	CoreEngine::CoreEngine(string configPath) : BlazeObject()
	{
		SetName("CoreEngine");

		this->configPath = configPath;
		config.LoadConfig(this->configPath);

		// Initialize SDL:
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			exit(-1);
		}
	};

	void CoreEngine::Startup()
	{
		// Initialize BlazeEngine:
		BlazeEventManager->Startup(this);		
		BlazeLogManager->Startup(this);

		BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "CoreEngine started!" }, true);
		BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN, this);

		BlazeTimeManager->Startup(this);
		BlazeInputManager->Startup(this);
		BlazeRenderManager->Startup(this);

		isRunning = true;

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "CoreEngine beginning main game loop!" });

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
		BlazeEventManager->Notify(new EventInfo{EVENT_LOG, this, "CoreEngine shutting down..."});

		config.SaveConfig("./");

		BlazeEventManager->Shutdown();
		BlazeTimeManager->Shutdown();		
		BlazeInputManager->Shutdown();
		BlazeRenderManager->Shutdown();

		BlazeEventManager->Update();

		return;
	}

	EngineConfig const * CoreEngine::GetConfig()
	{
		return &config;
	}

	int CoreEngine::HandleEvent(EventInfo const* eventInfo)
	{
		switch (eventInfo->type)
		{
		case EVENT_INPUT_BUTTON_DOWN:
			if (BlazeInputManager->GetInput(INPUT_BUTTON_QUIT))
			{
				Stop();
			}
			
			break;

		default:
			cout << "ERROR: Default event generated in CoreEngine!\n";
			break;
		}

		return 0;
	}

	void EngineConfig::LoadConfig(string path)
	{
		cout << "DEBUG: EngineConfig.LoadConfig is not implemented. Using hard coded default values!\n";
		
		windowName = "Blaze Engine";
		windowXRes = 800;
		windowYRes = 600;
	}

	void EngineConfig::SaveConfig(string path)
	{
		cout << "DEBUG: EngineConfig.SaveConfig is not implemented. No data is being saved!\n";
	}

}