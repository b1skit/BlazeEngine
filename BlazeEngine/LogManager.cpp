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
		LOG("Log manager started!");

		#if defined(DEBUG_LOGMANAGER_KEY_INPUT_LOGGING)
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_FORWARD, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_FORWARD, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_BACKWARD, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_BACKWARD, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_LEFT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_LEFT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_RIGHT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_RIGHT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_UP, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_UP, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_DOWN_DOWN, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_BUTTON_UP_DOWN, this);
			LOG("\tKey input logging enabled");
		#endif

		#if defined(DEBUG_LOGMANAGER_MOUSE_INPUT_LOGGING)
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_CLICK_LEFT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_RELEASE_LEFT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_CLICK_RIGHT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_RELEASE_RIGHT, this);
			CoreEngine::GetEventManager()->Subscribe(EVENT_INPUT_MOUSE_MOVED, this);
			LOG("\tMouse input logging enabled");
		#endif

		#if defined(DEBUG_LOGMANAGER_QUIT_LOGGING)
			CoreEngine::GetEventManager()->Subscribe(EVENT_ENGINE_QUIT, this);
			LOG("\tQuit event logging enabled");
		#endif
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
		#if defined(DEBUG_LOGMANAGER_LOG_EVENTS)
			string logMessage = EVENT_NAME[eventInfo->type] + ": Object #";

			if (eventInfo->generator)
			{
				logMessage += std::to_string(eventInfo->generator->GetObjectID()) + " (" + eventInfo->generator->GetName() + ")\t";
			}
			else
			{
				logMessage += "anonymous (     ??    )\t";
			}

			if (eventInfo->eventMessage && eventInfo->eventMessage->length() > 0)
			{
				logMessage += ": " + *eventInfo->eventMessage;
			}

			LOG(logMessage);
		#endif		
		
		return;
	}



	// Static functions:
	//------------------

	void BlazeEngine::LogManager::Log(string const& message)
	{
		if (message[0] == '\n')
		{
			cout << "\nLog:\t" << message.substr(1, string::npos) << "\n";
		}
		else if (message[0] == '\t')
		{
			cout << "\t" << message.substr(1, string::npos) << "\n";
		}
		else
		{
			cout << "Log:\t" << message << "\n";
		}
		

		// TODO: Implement writing to file
	}


	void BlazeEngine::LogManager::LogWarning(string const& message)
	{
		if (message[0] == '\n')
		{
			cout << "\nWarning:" << message.substr(1, string::npos) << "\n";
		}
		else
		{
			cout << "Warning:" << message << "\n";
		}


		// TODO: Implement writing to file
	}


	void BlazeEngine::LogManager::LogError(string const& message)
	{
		if (message[0] == '\n')
		{
			cout << "\nError:\t" << message.substr(1, string::npos) << "\n";
		}
		else
		{
			cout << "Error:\t" << message << "\n";
		}


		// TODO: Implement writing to file
	}



}