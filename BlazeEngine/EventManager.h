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
		EVENT_LOG,
		EVENT_ERROR,
		EVENT_ENGINE_QUIT,
		// EVENT_TICK ??
		// EVENT_UPDATE ??
		// ...

		EVENT_NUM_EVENTS // RESERVED: A count of the number of EVENT_TYPE's
	};
	

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
		
		// Singleton functionality:
		static EventManager& Instance();
		EventManager(EventManager const&) = delete; // Disallow copying of our Singleton
		void operator=(EventManager const&) = delete;
		
		// EngineComponent interface:
		void Startup(CoreEngine* coreEngine);
		void Shutdown();
		void Update();

		// Member functions:
		void Subscribe(EVENT_TYPE eventType, EventListener* listener); // Subscribe to an event
		/*void Unsubscribe(EventListener* listener);*/
		void Notify(EventInfo eventInfo, bool pushToFront = false); // Post an event

	private:
		vector< vector<EventInfo> > eventQueues;		
		vector< vector<EventListener*> > eventListeners;

	};


}