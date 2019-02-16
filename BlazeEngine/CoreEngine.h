#pragma once
#include <iostream>
#include "EventManager.h"
#include "LogManager.h"
#include "TimeManager.h"
#include "InputManager.h"
#include "RenderManager.h"

namespace BlazeEngine
{

	class CoreEngine : public BlazeObject, public EventListener
	{
	public:
		CoreEngine();

		// Engine component public API:		
		EventManager* const BlazeEventManager = &EventManager::Instance();
		TimeManager* const BlazeTimeManager = &TimeManager::Instance();
		InputManager* const BlazeInputManager = &InputManager::Instance();

		
		// Lifetime flow:
		void Startup();
		void Run();
		void Stop();
		void Shutdown();

		// EventListener interface:
		int HandleEvent(EventInfo const* eventInfo);


	private:	
		// Constants:
		const double FIXED_TIMESTEP = 1000.0 / 120.0; // Regular step size, in ms
		//const double MAX_TIMESTEP = 0.5;	// Max amount of time before giving up

		LogManager* const BlazeLogManager = &LogManager::Instance();
		RenderManager* const BlazeRenderManager = &RenderManager::Instance();

		// Engine control:
		bool isRunning = false;
	};
}
