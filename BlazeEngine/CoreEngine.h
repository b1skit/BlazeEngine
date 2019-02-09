#pragma once
#include <iostream>
#include "EventManager.h"
#include "LogManager.h"
#include "EventGenerator.h"

namespace BlazeEngine
{

	class CoreEngine : EventGenerator
	{
	public:
		// Engine component API:
		EventManager BlazeEventManager;
		LogManager BlazeLogManager;

		// Lifetime flow:
		void Startup();
		void Run();
		void Shutdown();

		// Utilities:
		int AssignObjectID();

		// EventGenerator:
		int GetObjectID();

	private:
		bool isRunning = false;
		int objectID;
		int objectIDs = 0;
	};
}
