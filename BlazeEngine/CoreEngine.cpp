#include <iostream>
#include "CoreEngine.h"
#include "EventManager.h"

using std::cout;


namespace BlazeEngine
{
	void CoreEngine::Startup()
	{
		objectID = AssignObjectID(); // CoreEngine should always run first to be given ID 0

		BlazeEventManager = EventManager::Instance();
		BlazeEventManager.Startup(this, AssignObjectID());
		
		BlazeLogManager = LogManager::Instance();
		BlazeLogManager.Startup(this, AssignObjectID());
		

		BlazeEventManager.Notify(EventInfo{EVENT_LOG, this, "CoreEngine started!" });

		isRunning = true;

		return;
	}


	// Main game loop
	void CoreEngine::Run()
	{
		BlazeEventManager.Notify(EventInfo{EVENT_LOG, this, "CoreEngine running!" });

		while (isRunning)
		{
			BlazeEventManager.Update();

			BlazeLogManager.Update();

			isRunning = false; // DEBUG
		}
	}

	void CoreEngine::Shutdown()
	{
		BlazeEventManager.Notify(EventInfo{EVENT_LOG, this, "CoreEngine shutting down..." });


		BlazeEventManager.Shutdown();

		return;
	}

	int CoreEngine::AssignObjectID() // CoreEngine should always access this first to be given ID 0
	{
		return objectIDs++;
	}

	int CoreEngine::GetObjectID()
	{
		return objectID;
	}


}