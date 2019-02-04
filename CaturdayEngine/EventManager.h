#pragma once
#include "EventListener.h"
#include "EventGenerator.h"
#include <queue>

using std::queue;

enum EVENT_TYPE
{
	UPDATE_EVENT = 0
	// ...

}; // EventManager.NUM_EVENT_TYPES must equal the number of enums


struct EventInfo
{
	EVENT_TYPE type;
	EventGenerator* generator;
};


class EventManager {
public:
	EventManager();

	~EventManager();

	// Subscribe to an event
	bool Subscribe(EVENT_TYPE eventType, EventListener* listener);

	// Post an event
	bool Notify(EVENT_TYPE eventType, EventGenerator* eventGenerator);



	
	static const int NUM_EVENT_TYPES = 1; // Must equal the count of enum EVENT_TYPE
private:	
	queue<EventInfo> eventQueues[NUM_EVENT_TYPES];

};


