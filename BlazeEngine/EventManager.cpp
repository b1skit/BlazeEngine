// Handles logging for the engine and 

#include "EventManager.h"
#include "BlazeObject.h"

using BlazeEngine::CoreEngine;


namespace BlazeEngine
{
	EventManager::EventManager() : EngineComponent()
	{
		coreEngine = nullptr;
		SetName("EventManager");

		eventQueues.reserve(NUM_EVENT_TYPES);
		for (int i = 0; i < NUM_EVENT_TYPES; i++)
		{
			eventQueues.push_back(vector<EventInfo>());
		}

		eventListeners.reserve(EVENT_QUEUE_START_SIZE);
		for (int i = 0; i < EVENT_QUEUE_START_SIZE; i++)
		{
			eventListeners.push_back(vector<EventListener*>());
		}		
	}

	//EventManager::~EventManager()
	//{
	//
	//}

	EventManager& EventManager::Instance()
	{
		static EventManager* instance = new EventManager();
		return *instance;
	}

	void EventManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		Notify(EventInfo{ EVENT_LOG, this, "Event manager started!" });
	}

	void EventManager::Shutdown()
	{
		Notify(EventInfo{ EVENT_LOG, this, "Event manager shutting down..." });
	}

	void EventManager::Update()
	{
		Notify(EventInfo{ EVENT_LOG, this, "Event manager updating!"});

		for (int currentEventType = 0; currentEventType < NUM_EVENT_TYPES; currentEventType++)
		{
			int numCurrentEvents = eventQueues[currentEventType].size();
			for (int currentEvent = 0; currentEvent < numCurrentEvents; currentEvent++)
			{
				int numListeners = eventListeners[currentEventType].size();
				for (int currentListener = 0; currentListener < numListeners; currentListener++)
				{
					eventListeners[currentEventType][currentListener]->HandleEvent(eventQueues[currentEventType][currentEvent]);
				}
			}

			// Clear the current queue:
			eventQueues[currentEventType].clear();
		}
	}

	void EventManager::Subscribe(EVENT_TYPE eventType, EventListener* listener)
	{
		eventListeners[eventType].push_back(listener);
		return;
	}

	//void EventManager::Unsubscribe(EventListener * listener)
	//{
	//	// DEBUG:
	//	Notify(EventInfo{ EVENT_ERROR, this, "EventManager.Unsubscribe() was called, but is NOT implemented!"});

	//	return;
	//}

	void EventManager::Notify(EventInfo eventInfo, bool pushToFront)
	{
		if (pushToFront)
		{
			vector<EventInfo>::iterator iterator = eventQueues[(int)eventInfo.type].begin();
			eventQueues[(int)eventInfo.type].insert(iterator, eventInfo);
		}
		else
		{
			eventQueues[(int)eventInfo.type].push_back(eventInfo);
		}
		return;
	}
}