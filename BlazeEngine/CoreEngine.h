#pragma once
#include <iostream>
#include "EventManager.h"
#include "LogManager.h"
#include "TimeManager.h"

namespace BlazeEngine
{

	class CoreEngine : public BlazeObject
	{
	public:
		/*CoreEngine() : BlazeObject() 
		{
			SetName("CoreEngine");
		};*/

		CoreEngine();

		// Engine component public API:
		EventManager* BlazeEventManager;
		TimeManager* BlazeTimeManager;

		// Lifetime flow:
		void Startup();
		void Run();
		void Shutdown();


	private:
		// Engine config:
		int maxFPS = 150;
		
		// Constants:
		const double FIXED_TIMESTEP = 1000.0 / 120.0; // Regular step size, in ms
		//const double MAX_TIMESTEP = 0.5;	// Max amount of time before giving up


		// Private engine components:
		LogManager* BlazeLogManager;
		
		// Engine control:
		bool isRunning = false;
	};
}
