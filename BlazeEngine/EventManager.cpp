#include "EventManager.h"
//#include "CoreEngine.h"
//#include "EngineComponent.h"

#include <iostream> // DEBUG
#include <string>
using std::cout; // DEBUG
using std::to_string;

using BlazeEngine::CoreEngine;


namespace BlazeEngine
{

	//EventManager::EventManager()
	//{
	//	//eventQueues[UPDATE_EVENT] = queue<

	//}

	//EventManager::~EventManager()
	//{

	//}

	EventManager& EventManager::Instance()
	{
		cout << "EventManager.Instance() called!\n"; // To do: Log this as an event...
		
		static EventManager* instance = new EventManager();
		return *instance;
	}

	void EventManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::coreEngine = coreEngine;

		cout << "EventManager.Startup() called!\n"; // To do: Log this as an event...
	}

	void EventManager::Shutdown()
	{
		cout << "EventManager.Shutdown() called!\n"; // To do: Log this as an event...
	}

	void EventManager::Update()
	{
		cout << "EventManager.Update() called!\n"; // To do: Log this as an event...
	}

	bool EventManager::Subscribe(EVENT_TYPE eventType, EventListener* listener)
	{
		//BlazeEventManager
		return true;
	}

	bool EventManager::Notify(EVENT_TYPE eventType, EventGenerator* eventGenerator)
	{
		EventInfo newEvent{ eventType, eventGenerator };
		eventQueues[eventType].push(newEvent);

		return true;
	}

}