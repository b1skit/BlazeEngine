#pragma once
#include "EventManager.h"
#include "LogManager.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "SceneManager.h"
#include "EngineConfig.h"


namespace BlazeEngine
{

	// CORE ENGINE:
	class CoreEngine : public BlazeObject, public EventListener
	{
	public:
		CoreEngine(int argc, char** argv);

		// Static Engine component singletons getters:		
		static inline CoreEngine*		GetCoreEngine()		{ return coreEngine; }
		static inline EventManager*		GetEventManager()	{ return BlazeEventManager; }
		static inline InputManager*		GetInputManager()	{ return BlazeInputManager; }
		static inline SceneManager*		GetSceneManager()	{ return BlazeSceneManager; }
		static inline RenderManager*	GetRenderManager()	{ return BlazeRenderManager; }
		
		// Lifetime flow:
		void Startup();
		void Run();
		void Stop();
		void Shutdown();

		// Member functions
		EngineConfig const* GetConfig();

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		
	private:	
		// Constants:
		const double FIXED_TIMESTEP = 1000.0 / 120.0; // Regular step size, in ms
		//const double MAX_TIMESTEP = 0.5;	// Max amount of time before giving up

		// Private engine component singletons:	
		LogManager* const	BlazeLogManager		= &LogManager::Instance();
		TimeManager* const	BlazeTimeManager	= &TimeManager::Instance();

		// Static Engine component singletons
		static CoreEngine*		coreEngine;
		static EventManager*	BlazeEventManager;
		static InputManager*	BlazeInputManager;
		static SceneManager*	BlazeSceneManager;
		static RenderManager*	BlazeRenderManager;
		

		// Engine control:
		bool isRunning = false;

		// Engine configuration:
		EngineConfig config;

		bool ProcessCommandLineArgs(int argc, char** argv);
	};
}
