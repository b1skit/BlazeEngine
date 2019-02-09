#pragma once
//#include "CoreEngine.h"
#include "EventListener.h"
#include "EventGenerator.h"
#include "EngineComponent.h"
#include <queue>

using std::queue;

namespace BlazeEngine
{
	struct EventInfo
	{
		EVENT_TYPE type;
		EventGenerator* generator;
	};


	class EventManager : public EngineComponent
	{
	public:
		//EventManager();
		//~EventManager();
		static EventManager& Instance();

		void Startup(CoreEngine* coreEngine);

		void Shutdown();

		void Update();

		// Subscribe to an event
		bool Subscribe(EVENT_TYPE eventType, EventListener* listener);

		// Post an event
		bool Notify(EVENT_TYPE eventType, EventGenerator* eventGenerator);



		static const int NUM_EVENT_TYPES = 1; // Must equal the count of enum EVENT_TYPE

	private:


		

		queue<EventInfo> eventQueues[NUM_EVENT_TYPES];

	};


}