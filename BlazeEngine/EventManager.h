#pragma once
#include "EventListener.h"
#include "EventGenerator.h"
#include "EngineComponent.h"
#include <vector>

using std::vector;


namespace BlazeEngine
{
	const static int EVENT_QUEUE_START_SIZE = 100; // The starting size of the event queue to reserve

	class EventManager : public EngineComponent, public EventGenerator
	{
	public:
		EventManager();
		//~EventManager();
		static EventManager& Instance();

		void Startup(CoreEngine* coreEngine);

		void Shutdown();

		void Update();

		// Subscribe to an event
		void Subscribe(EVENT_TYPE eventType, EventListener* listener);

		void Unsubscribe(EventListener* listener);

		// Post an event
		void Notify(EventInfo eventInfo);

	private:
		vector< vector<EventInfo> > eventQueues;		
		vector< vector<EventListener*> > eventListeners;

	};


}