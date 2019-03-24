//#include <iostream>
#include "CoreEngine.h"

#include "SDL.h"

#include <string>

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	// Static members:
	CoreEngine* CoreEngine::coreEngine; // Assigned in constructor

	EventManager* CoreEngine::BlazeEventManager = &EventManager::Instance();
	InputManager* CoreEngine::BlazeInputManager = &InputManager::Instance();
	SceneManager* CoreEngine::BlazeSceneManager = &SceneManager::Instance();
	RenderManager* CoreEngine::BlazeRenderManager = &RenderManager::Instance();


	CoreEngine::CoreEngine(string configPath) : BlazeObject("CoreEngine")
	{
		coreEngine = this;

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
		BlazeEventManager->Startup();	
		BlazeLogManager->Startup();

		BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("CoreEngine started!") }, true);
		BlazeEventManager->Subscribe(EVENT_ENGINE_QUIT, this);

		BlazeTimeManager->Startup();
		BlazeInputManager->Startup();

		BlazeRenderManager->Startup();

		// Must wait to start scene manager and load a scene until the renderer is called, since we need to initialize OpenGL in the RenderManager before creating shaders
		BlazeSceneManager->Startup();
		BlazeSceneManager->LoadScene(config.scene.scenePath);

		isRunning = true;

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, new string("CoreEngine beginning main game loop!") });

		// Initialize game loop timing:
		double elapsed = 0.0;
		BlazeTimeManager->Update();

		while (isRunning)
		{
			BlazeInputManager->Update(); // Note: Input processing occurs via events

			BlazeTimeManager->Update();
			elapsed += BlazeTimeManager->DeltaTime();

			while (elapsed >= FIXED_TIMESTEP)
			{
				// Update components:
				BlazeEventManager->Update(); // Clears SDL event queue: Must occur after any other component that listens to SDL events
				BlazeLogManager->Update();

				BlazeSceneManager->Update(); // Updates all of the scene objects

				// Update the time for the next iteration:
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
		BlazeEventManager->Notify(new EventInfo{EVENT_LOG, this, new string("CoreEngine shutting down...") });

		if (configDirty)
		{
			config.SaveConfig(configPath);
		}		
		
		// Note: Shutdown order matters!
		BlazeTimeManager->Shutdown();		
		BlazeInputManager->Shutdown();
		
		BlazeRenderManager->Shutdown();
		
		BlazeSceneManager->Shutdown();
		
		BlazeEventManager->Shutdown();

		BlazeLogManager->Shutdown();
		

		return;
	}

	EngineConfig const * CoreEngine::GetConfig()
	{
		return &config;
	}

	void CoreEngine::HandleEvent(EventInfo const* eventInfo)
	{
		switch (eventInfo->type)
		{
		case EVENT_ENGINE_QUIT:
		{
			Stop();
		}			
			break;

		default:
			BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, new string("ERROR: Default event generated in CoreEngine!") });
			break;
		}

		return;
	}


	void EngineConfig::LoadConfig(string path)
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, nullptr, new string("EngineConfig.LoadConfig() is not implemented. Using hard coded default values!") });
	
		// TO DO: Implement config loading!
	}

	void EngineConfig::SaveConfig(string path)
	{
		CoreEngine::GetEventManager()->Notify(new EventInfo{ EVENT_DEBUG, nullptr, new string("EngineConfig.SaveConfig() is not implemented. No data is being saved!\n") });
	}
}