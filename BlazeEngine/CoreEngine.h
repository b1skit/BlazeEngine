#pragma once
#include <iostream>
//#include "EngineComponent.h"
#include "EventManager.h"
#include "LogManager.h"

namespace BlazeEngine
{

	class CoreEngine
	{
	public:
		// Engine component API:
		EventManager BlazeEventManager;
		LogManager BlazeLogManager;

		
		void Startup();

		void Run();

		void Shutdown();

	private:
	};
}
