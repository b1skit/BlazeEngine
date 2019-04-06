#include "LogManager.h"
#include "EventManager.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#include <iostream>
using std::cout;

namespace BlazeEngine
{
	//LogManager::~LogManager()
	//{
	//
	//}

	LogManager& LogManager::Instance()
	{
		static LogManager* instance = new LogManager();
		return *instance;
	}

	void LogManager::Startup() 
	{
		//// Subscribe to every event type:
		//#if defined(LOG_VERBOSITY_DEBUG) || defined(LOG_VERBOSITY_ALL)
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_ENGINE_QUIT, this);
		//#endif	

		//#if defined(LOG_VERBOSITY_ALL)
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_FORWARD, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_FORWARD, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_BACKWARD, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_BACKWARD, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_LEFT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_LEFT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_RIGHT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_RIGHT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_UP, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_UP, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_DOWN, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_DOWN, this);
		//
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_CLICK_LEFT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_RELEASE_LEFT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_CLICK_RIGHT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_RELEASE_RIGHT, this);
		//	CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_MOVED, this);
		//#endif

		LOG("Log manager started!");		
	}

	void LogManager::Shutdown()
	{
		LOG("Log manager shutting down...");
	}

	void LogManager::Update()
	{
		
	}


	void LogManager::HandleEvent(EventInfo const* eventInfo)
	{
		/*#if defined(DEBUG_LOG_MANAGER_PRINT_EVENTS)
			LOG(EVENT_NAME[eventInfo->type] + ": Object #");

			if (eventInfo->generator)
			{
				LOG(std::to_string(eventInfo->generator->GetObjectID()) + " (" + eventInfo->generator->GetName() + ")\t");
			}
			else
			{
				LOG("anonymous (     ??    )\t");
			}

			if (eventInfo->eventMessage && eventInfo->eventMessage->length() > 0)
			{
				LOG(": " + *eventInfo->eventMessage);
			}
		#endif		*/
		
		return;
	}



	// Static functions:
	//------------------

	void BlazeEngine::LogManager::Log(string const& message)
	{
		cout << "Log:\t" << message << "\n";

		// TO DO: Implement writing to file
	}


	void BlazeEngine::LogManager::LogError(string const& message)
	{
		cout << "Error:\t" << message << "\n";

		// TO DO: Implement writing to file
	}
}