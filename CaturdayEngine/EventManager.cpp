#include "EventManager.h"

namespace BlazeEngine
{

	//EventManager::EventManager()
	//{
	//	//eventQueues[UPDATE_EVENT] = queue<
	//}

	//EventManager::~EventManager()
	//{

	//}

	bool EventManager::Subscribe(EVENT_TYPE eventType, EventListener* listener)
	{

		return true;
	}

	bool EventManager::Notify(EVENT_TYPE eventType, EventGenerator* eventGenerator)
	{
		EventInfo newEvent{ eventType, eventGenerator };
		eventQueues[eventType].push(newEvent);

		return true;
	}

}