// Blaze Engine Event Generator

#pragma once
#include "EventListener.h"
#include "EngineComponent.h"
#include <vector>

using std::vector;


namespace BlazeEngine
{

	const static int EVENT_QUEUE_START_SIZE = 100; // The starting size of the event queue to reserve

	enum EVENT_TYPE
	{
		EVENT_LOG = 0,
		EVENT_ERROR = 1,
		// EVENT_TICK ??
		// EVENT_UPDATE ??
		// ...
	};
	const static int NUM_EVENT_TYPES = 2; // MUST equal the number of EVENT_TYPE enums

	struct EventInfo
	{
		EVENT_TYPE type;
		BlazeObject* generator;
		string eventMessage = ""; // Default to empty message
	};


	class EventManager : public EngineComponent
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