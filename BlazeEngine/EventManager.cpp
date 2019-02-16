// Handles logging for the engine and 

#include "EventManager.h"
#include "BlazeObject.h"

using BlazeEngine::CoreEngine;


namespace BlazeEngine
{
	EventManager::EventManager() : EngineComponent()
	{
		SetName("EventManager");

		eventQueues.reserve(EVENT_NUM_EVENTS);
		for (int i = 0; i < EVENT_NUM_EVENTS; i++)
		{
			eventQueues.push_back(vector<EventInfo const*>());
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

		Notify(new EventInfo{ EVENT_LOG, this, "Event manager started!" });
	}

	void EventManager::Shutdown()
	{
		Notify(new EventInfo{ EVENT_LOG, this, "Event manager shutting down..." });
	}

	void EventManager::Update()
	{
		// Loop through each type of event:
		for (int currentEventType = 0; currentEventType < EVENT_NUM_EVENTS; currentEventType++)
		{
			// Loop through each event item in the current event queue:
			size_t numCurrentEvents = eventQueues[currentEventType].size();
			for (int currentEvent = 0; currentEvent < numCurrentEvents; currentEvent++)
			{
				// Loop through each listener subscribed to the current event:
				size_t numListeners = eventListeners[currentEventType].size();
				for (int currentListener = 0; currentListener < numListeners; currentListener++)
				{
					eventListeners[currentEventType][currentListener]->HandleEvent(eventQueues[currentEventType][currentEvent]);
				}
				
				// Deallocate the event:
				delete eventQueues[currentEventType][currentEvent];
			}

			// Clear the current event queue (of now invalid pointers):
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

	void EventManager::Notify(EventInfo const* eventInfo, bool pushToFront)
	{
		if (pushToFront)
		{
			vector<EventInfo const*>::iterator iterator = eventQueues[(int)eventInfo->type].begin();
			eventQueues[(int)eventInfo->type].insert(iterator, eventInfo);
		}
		else
		{
			eventQueues[(int)eventInfo->type].push_back(eventInfo);
		}
		return;
	}
}