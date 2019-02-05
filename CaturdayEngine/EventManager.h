#pragma once
#include "EventListener.h"
#include "EventGenerator.h"
#include <queue>

using std::queue;

namespace CaturdayEngine
{
	struct EventInfo
	{
		EVENT_TYPE type;
		EventGenerator* generator;
	};


	class EventManager {
	public:
		static EventManager& Instance()
		{
			static EventManager* instance = new EventManager();
			return *instance;
		}

		// Subscribe to an event
		bool Subscribe(EVENT_TYPE eventType, CaturdayEngine::EventListener* listener);

		// Post an event
		bool Notify(EVENT_TYPE eventType, EventGenerator* eventGenerator);




		static const int NUM_EVENT_TYPES = 1; // Must equal the count of enum EVENT_TYPE
	private:
		//EventManager();

		//~EventManager();

		queue<EventInfo> eventQueues[NUM_EVENT_TYPES];

	};


}