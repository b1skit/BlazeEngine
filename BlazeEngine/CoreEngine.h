#pragma once
#include <iostream>
#include "EventManager.h"
#include "LogManager.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "SceneManager.h"

namespace BlazeEngine
{
	// TO DO: Implement loading/saving of this object...
	struct EngineConfig
	{
		struct
		{
			string windowTitle;
			int windowXRes;
			int windowYRes;
		} renderer;
		

		// TO DO: Add button config

		// TO DO: Implement load/save functions
		void LoadConfig(string path);
		void SaveConfig(string path);

	};

	class CoreEngine : public BlazeObject, public EventListener
	{
	public:
		CoreEngine(string configPath);

		// Engine component singletons public API:		
		EventManager* const BlazeEventManager = &EventManager::Instance();
		TimeManager* const BlazeTimeManager = &TimeManager::Instance();
		InputManager* const BlazeInputManager = &InputManager::Instance();
		SceneManager* const BlazeSceneManager = &SceneManager::Instance();

		
		// Lifetime flow:
		void Startup();
		void Run();
		void Stop();
		void Shutdown();

		// Member functions
		EngineConfig const* GetConfig();

		// BlazeObject interface:
		void Update() {} // Do nothing...

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);


	private:	
		// Constants:
		const double FIXED_TIMESTEP = 1000.0 / 120.0; // Regular step size, in ms
		//const double MAX_TIMESTEP = 0.5;	// Max amount of time before giving up

		// Private engine component singletons:	
		LogManager* const BlazeLogManager = &LogManager::Instance();
		RenderManager* const BlazeRenderManager = &RenderManager::Instance();

		// Engine control:
		bool isRunning = false;

		// Engine configuration:
		EngineConfig config;
		string configPath;
		bool configDirty = false; // Marks whether we need to save the config file or not
	};
}
