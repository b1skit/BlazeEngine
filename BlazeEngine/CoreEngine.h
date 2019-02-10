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
		CoreEngine() : BlazeObject() 
		{
			SetName("CoreEngine");
		};

		// Engine component public API:
		EventManager* BlazeEventManager;
		TimeManager* BlazeTimeManager;

		// Lifetime flow:
		void Startup();
		void Run();
		void Shutdown();


	private:
		bool isRunning = false;
		int objectIDs = 0;

		// Private engine components:
		LogManager* BlazeLogManager;
		
	};
}
