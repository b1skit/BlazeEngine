#include <iostream>
#include "CoreEngine.h"
#include "EventManager.h"

using std::cout;


namespace BlazeEngine
{
	void CoreEngine::Startup()
	{
		BlazeEventManager = &EventManager::Instance();
		BlazeEventManager->Startup(this);
		
		BlazeLogManager = &LogManager::Instance();
		BlazeLogManager->Startup(this);

		BlazeEventManager->Notify(EventInfo{EVENT_LOG, this, "CoreEngine started!" }, true);

		isRunning = true;

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		BlazeEventManager->Notify(EventInfo{EVENT_LOG, this, "CoreEngine running!" });

		while (isRunning)
		{
			BlazeEventManager->Update();

			BlazeLogManager->Update();

			isRunning = false; // DEBUG
		}
	}

	void CoreEngine::Shutdown()
	{
		BlazeEventManager->Notify(EventInfo{EVENT_LOG, this, "CoreEngine shutting down..." });


		BlazeEventManager->Shutdown();

		return;
	}
}