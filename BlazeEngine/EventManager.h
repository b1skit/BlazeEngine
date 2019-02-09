#pragma once
#include "EventListener.h"
#include "EventGenerator.h"
#include "EngineComponent.h"
#include <vector>
//#include <queue>


//using std::queue;
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

		void Startup(CoreEngine* coreEngine, int objectID);

		void Shutdown();

		void Update();

		// Subscribe to an event
		void Subscribe(EVENT_TYPE eventType, EventListener* listener);

		void Unsubscribe(EventListener* listener);

		// Post an event
		void Notify(EventInfo eventInfo);

		// EventGenerator:
		int GetObjectID();



		//static const int NUM_EVENT_TYPES = 1; // Must equal the count of enum EVENT_TYPE

	private:
		
		//vector< vector<EventGenerator*> > eventQueues;
		vector<vector<EventInfo>> eventQueues;
		
		vector< vector<EventListener*> > eventListeners;

	};


}