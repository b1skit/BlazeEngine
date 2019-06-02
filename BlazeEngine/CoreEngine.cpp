#include "CoreEngine.h"
#include "BuildConfiguration.h"

#include "SDL.h"

#include <string>


namespace BlazeEngine
{
	// Static members:
	CoreEngine* CoreEngine::coreEngine; // Assigned in constructor

	EventManager* CoreEngine::BlazeEventManager = &EventManager::Instance();
	InputManager* CoreEngine::BlazeInputManager = &InputManager::Instance();
	SceneManager* CoreEngine::BlazeSceneManager = &SceneManager::Instance();
	RenderManager* CoreEngine::BlazeRenderManager = &RenderManager::Instance();


	CoreEngine::CoreEngine(int argc, char** argv) : BlazeObject("CoreEngine")
	{
		coreEngine = this;

		EngineConfig config = EngineConfig();

		ProcessCommandLineArgs(argc, argv);

		//this->configPath = configPath;
		//config.LoadConfig(this->configPath);

		// Initialize SDL:
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			exit(-1);
		}
		// TODO: Initialize per-component, instead of initializing everything???
	}


	void CoreEngine::Startup()
	{
		// Initialize BlazeEngine:
		BlazeEventManager->Startup();	
		BlazeLogManager->Startup();

		LOG("CoreEngine started!");
		BlazeEventManager->Subscribe(EVENT_ENGINE_QUIT, this);

		BlazeTimeManager->Startup();
		BlazeInputManager->Startup();

		BlazeRenderManager->Startup();

		// Must wait to start scene manager and load a scene until the renderer is called, since we need to initialize OpenGL in the RenderManager before creating shaders
		BlazeSceneManager->Startup();
		BlazeSceneManager->LoadScene(config.scene.currentScene);

		// Now that the scene (and its materials/shaders) has been loaded, we can initialize the shaders
		BlazeRenderManager->InitializeShaders();

		isRunning = true;

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		LOG("CoreEngine beginning main game loop!");

		// Process any events that might have occurred during startup:
		BlazeEventManager->Update();
		BlazeLogManager->Update();

		// Initialize game loop timing:
		double elapsed = 0.0;
		BlazeTimeManager->Update();

		while (isRunning)
		{
			BlazeInputManager->Update(); // Note: Input processing occurs via events. This just resets the mouse state

			BlazeTimeManager->Update();
			elapsed += BlazeTimeManager->DeltaTime();

			while (elapsed >= FIXED_TIMESTEP)
			{
				//BlazeInputManager->Update(); // Note: Input processing occurs via events. This just resets the mouse state

				// Update components:
				BlazeEventManager->Update(); // Clears SDL event queue: Must occur after any other component that listens to SDL events
				BlazeLogManager->Update();

				BlazeSceneManager->Update(); // Updates all of the scene objects

				// Update the time for the next iteration:
				elapsed -= FIXED_TIMESTEP;
			}
			
			BlazeRenderManager->Update();
		}
	}


	void CoreEngine::Stop()
	{
		isRunning = false;
	}


	void CoreEngine::Shutdown()
	{
		LOG("CoreEngine shutting down...");

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

		delete BlazeTimeManager;
		delete BlazeInputManager;
		delete BlazeRenderManager;
		delete BlazeSceneManager;
		delete BlazeEventManager;
		delete BlazeLogManager;		

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
			LOG_ERROR("ERROR: Default event generated in CoreEngine!");
			break;
		}

		return;
	}


	void EngineConfig::LoadConfig(string path)
	{
		LOG_WARNING("DEBUG: EngineConfig.LoadConfig() is not implemented. Using hard coded default values!");
	
		// TODO: Implement config loading!
	}


	void EngineConfig::SaveConfig(string path)
	{
		LOG_WARNING("DEBUG: EngineConfig.SaveConfig() is not implemented. No data is being saved!\n");
	}

	void CoreEngine::ProcessCommandLineArgs(int argc, char** argv)
	{
		LOG("Processing " + to_string(argc - 1) + " command line arguments:");
		for (int i = 1; i < argc; i++) // Start at index 1 to skip the executable path
		{			
			string currentArg = string(argv[i]);			
			if (currentArg.find("-scene") != string::npos)
			{
				string parameter = string(argv[i + 1]);
				if (i < argc - 1)
				{
					LOG("\tReceived scene command: \"" + currentArg + " " + parameter + "\"");

					this->config.scene.currentScene = parameter;
				}
				
				i++; // Eat the extra command parameter
			}
			else
			{
				LOG_ERROR("\"" + currentArg + "\" is not a recognized command!");
			}

		}
	}
}