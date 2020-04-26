// Handles logging for the engine and 

#include "EventManager.h"
#include "BlazeObject.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"
#include "EventListener.h"

#include "SDL.h"


namespace BlazeEngine
{
	EventManager::EventManager() : EngineComponent("EventManager")
	{
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


	void EventManager::Startup()
	{
		LOG("Event manager started!");
	}


	void EventManager::Shutdown()
	{
		Update(); // Run one last update

		LOG("Event manager shutting down...");
	}


	void EventManager::Update()
	{
		// Check for SDL quit events (only). We do this instead of parsing the entire queue with SDL_PollEvent(), which removed input events we needed
		SDL_PumpEvents();
		
		#define NUM_EVENTS 1
		SDL_Event eventBuffer[NUM_EVENTS]; // 
		if (SDL_PeepEvents(eventBuffer, NUM_EVENTS, SDL_GETEVENT, SDL_QUIT, SDL_QUIT) > 0)
		{
			this->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Received SDL_QUIT event") });
		}

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
				if (eventQueues[currentEventType][currentEvent]->eventMessage != nullptr)
				{
					delete eventQueues[currentEventType][currentEvent]->eventMessage;
				}
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


	void EventManager::Notify(EventInfo const* eventInfo, bool pushToFront /*= false*/)
	{
		#if defined(DEBUG_PRINT_NOTIFICATIONS)
			if (eventInfo)
			{
				if (eventInfo->generator)
				{
					if (eventInfo->eventMessage)
					{
						LOG("NOTIFICATION: " + to_string((long long)eventInfo->generator) + " : " + *eventInfo->eventMessage);
					}
					else
					{
						LOG("NOTIFICATION: " + to_string((long long)eventInfo->generator) + " : nullptr");
					}
				}
				else
				{
					if (eventInfo->eventMessage)
					{
						LOG("NOTIFICATION: nullptr : " + *eventInfo->eventMessage);
					}
					else
					{
						LOG("NOTIFICATION: nullptr : nullptr");
					}
				}
			}
			else
			{
				LOG("NOTIFICATION: Received NULL eventInfo...");
			}			
		#endif

		// Select what to notify based on type?

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