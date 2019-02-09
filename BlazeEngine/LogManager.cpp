#include "LogManager.h"
#include <iostream>
#include "EventManager.h"

using std::cout;


namespace BlazeEngine
{
	//LogManager::LogManager()
	//{
	//	_EventManager.Instance();
	//}


	//LogManager::~LogManager()
	//{
	//
	//}

	LogManager& LogManager::Instance()
	{
		cout << "LogManager.Instance() called!\n"; // To do: Log this as an event...

		static LogManager* instance = new LogManager();
		return *instance;
	}

	void LogManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::coreEngine = coreEngine;

		cout << "LogManager.Startup() called!\n"; // To do: Log this as an event...
	}

	void LogManager::Shutdown()
	{
		cout << "LogManager.Shutdown() called!\n"; // To do: Log this as an event...
	}

	void LogManager::Update()
	{
		cout << "LogManager.Update() called!\n"; // To do: Log this as an event...
	}


	int LogManager::HandleEvent(EVENT_TYPE event, EventGenerator * generator)
	{
		switch (event)
		{
		case EVENT_TEST:
			cout << "EVENT_TEST posted by generator  ???\n";
			break;

		default:
			cout << "ERROR: Default event generated!\n";
			break;
		}

		return 0;
	}

}