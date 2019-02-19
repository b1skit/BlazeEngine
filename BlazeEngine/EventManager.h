// Blaze Engine Event Generator

#pragma once
#include "EventListener.h"
#include "EngineComponent.h"
#include <vector>
#include "SDL.h"

using std::vector;


namespace BlazeEngine
{

	const static int EVENT_QUEUE_START_SIZE = 100; // The starting size of the event queue to reserve

	enum EVENT_TYPE
	{
		// System:
		EVENT_LOG,
		EVENT_ERROR,
		EVENT_ENGINE_QUIT,
		
		// Button inputs:
		EVENT_INPUT_BUTTON_DOWN_FORWARD,
		EVENT_INPUT_BUTTON_UP_FORWARD,
		EVENT_INPUT_BUTTON_DOWN_BACKWARD,
		EVENT_INPUT_BUTTON_UP_BACKWARD,
		EVENT_INPUT_BUTTON_DOWN_LEFT,
		EVENT_INPUT_BUTTON_UP_LEFT,
		EVENT_INPUT_BUTTON_DOWN_RIGHT,
		EVENT_INPUT_BUTTON_UP_RIGHT,
		EVENT_INPUT_BUTTON_DOWN_UP,
		EVENT_INPUT_BUTTON_UP_UP,
		EVENT_INPUT_BUTTON_DOWN_DOWN,
		EVENT_INPUT_BUTTON_UP_DOWN,
		
		// Mouse inputs:
		EVENT_INPUT_MOUSE_CLICK_LEFT,
		EVENT_INPUT_MOUSE_RELEASE_LEFT,
		EVENT_INPUT_MOUSE_CLICK_RIGHT,
		EVENT_INPUT_MOUSE_RELEASE_RIGHT,
		EVENT_INPUT_MOUSE_MOVED,

		// EVENT_TICK ??
		// EVENT_UPDATE ??
		// ...

		EVENT_NUM_EVENTS // RESERVED: A count of the number of EVENT_TYPE's
	};
	
	// Matched event string names:
	const static string EVENT_NAME[EVENT_NUM_EVENTS] = 
	{
		// System:
		"EVENT_LOG", 
		"EVENT_ERROR", 
		"EVENT_ENGINE_QUIT", 

		// Button inputs:
		"EVENT_INPUT_BUTTON_DOWN_FORWARD",
		"EVENT_INPUT_BUTTON_UP_FORWARD",
		"EVENT_INPUT_BUTTON_DOWN_BACKWARD",
		"EVENT_INPUT_BUTTON_UP_BACKWARD",
		"EVENT_INPUT_BUTTON_DOWN_LEFT",
		"EVENT_INPUT_BUTTON_UP_LEFT",
		"EVENT_INPUT_BUTTON_DOWN_RIGHT",
		"EVENT_INPUT_BUTTON_UP_RIGHT",
		"EVENT_INPUT_BUTTON_DOWN_UP",
		"EVENT_INPUT_BUTTON_UP_UP",
		"EVENT_INPUT_BUTTON_DOWN_DOWN",
		"EVENT_INPUT_BUTTON_UP_DOWN",

		// Mouse inputs:
		"EVENT_INPUT_MOUSE_CLICK_LEFT",
		"EVENT_INPUT_MOUSE_RELEASE_LEFT",
		"EVENT_INPUT_MOUSE_CLICK_RIGHT",
		"EVENT_INPUT_MOUSE_RELEASE_RIGHT",
		"EVENT_INPUT_MOUSE_MOVED",

	}; // NOTE: String order must match the order of EVENT_TYPE enum


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
		void Notify(EventInfo const* eventInfo, bool pushToFront = false); // Post an event

	private:
		vector< vector<EventInfo const* > > eventQueues;
		vector< vector<EventListener*> > eventListeners;

		// SDL2 event queue handling:
		const static int MAX_EVENTS = 1; // Max number of events to look for
		SDL_Event SDLEventBuffer[MAX_EVENTS];

	};


}