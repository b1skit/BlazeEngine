#include <iostream>
#include "CoreEngine.h"
#include "EventManager.h"

using std::cout;


namespace BlazeEngine
{
	void CoreEngine::Startup()
	{
		cout << "CoreEngine starting up...\n";

		BlazeEventManager = EventManager::Instance();
		BlazeEventManager.Startup(this);

		BlazeLogManager = LogManager::Instance();
		BlazeLogManager.Startup(this);

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		cout << "CoreEngine Run() called!\n";
	}

	void CoreEngine::Shutdown()
	{
		cout << "CoreEngine shutting down...\n";

		BlazeEventManager.Shutdown();

		return;
	}


}