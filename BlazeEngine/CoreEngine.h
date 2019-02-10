#pragma once
#include <iostream>
#include "EventManager.h"
#include "LogManager.h"

namespace BlazeEngine
{

	class CoreEngine : public BlazeObject
	{
	public:
		CoreEngine() : BlazeObject() 
		{
			SetName("CoreEngine");
		};

		// Engine component API:
		EventManager* BlazeEventManager;
		LogManager* BlazeLogManager;

		// Lifetime flow:
		void Startup();
		void Run();
		void Shutdown();


	private:
		bool isRunning = false;
		int objectIDs = 0;
	};
}
