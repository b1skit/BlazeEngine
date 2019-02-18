// Handles logging for the engine and 

#include "EventManager.h"
#include "BlazeObject.h"
#include "CoreEngine.h"
#include "SDL.h"

using BlazeEngine::CoreEngine;


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

	void EventManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		Notify(new EventInfo{ EVENT_LOG, this, "Event manager started!" });
	}

	void EventManager::Shutdown()
	{
		Update(); // Run one last update

		Notify(new EventInfo{ EVENT_LOG, this, "Event manager shutting down..." });
	}

	void EventManager::Update()
	{
		bool doFireEvent = true;

		// Catch relevant events from SDL2:
		SDL_Event currentSDLEvent;
		while (SDL_PollEvent(&currentSDLEvent))
		{
			switch (currentSDLEvent.type)
			{
			case SDL_QUIT:
			{
				Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, "Received SDL_QUIT event" });
				break;
			}

			case SDL_KEYDOWN:
			{
				int numKeys = 0;
				const Uint8* keyboardState = SDL_GetKeyboardState(&numKeys);
				InputBindings const* inputBindings = coreEngine->BlazeInputManager->GetInputBindings();
				if (numKeys > 0)
				{
					// Button input events:
					if (currentSDLEvent.key.keysym.sym == inputBindings->Button_forward)
					{
						if (!coreEngine->BlazeInputManager->GetInput(INPUT_BUTTON_FORWARD))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_FORWARD, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_backward)
					{
						if (!coreEngine->BlazeInputManager->GetInput(INPUT_BUTTON_BACKWARD))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_BACKWARD, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_left)
					{
						if (!coreEngine->BlazeInputManager->GetInput(INPUT_BUTTON_LEFT))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_LEFT, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_right)
					{
						if (!coreEngine->BlazeInputManager->GetInput(INPUT_BUTTON_RIGHT))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_RIGHT, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_up)
					{
						if (!coreEngine->BlazeInputManager->GetInput(INPUT_BUTTON_UP))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_UP, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_down)
					{
						if (!coreEngine->BlazeInputManager->GetInput(INPUT_BUTTON_DOWN))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_DOWN, this });
						}
					}

					// Quit button
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_quit)
					{
						Notify(new EventInfo{ EVENT_ENGINE_QUIT, this });
					}
				}
				break;
			}
			case SDL_KEYUP:
			{
				int numKeys = 0;
				const Uint8* keyboardState = SDL_GetKeyboardState(&numKeys);
				InputBindings const* inputBindings = coreEngine->BlazeInputManager->GetInputBindings();
				if (numKeys > 0)
				{
					// Button input events:
					if (currentSDLEvent.key.keysym.sym == inputBindings->Button_forward)
					{
						Notify(new EventInfo{ EVENT_INPUT_BUTTON_UP_FORWARD, this });
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_backward)
					{
						Notify(new EventInfo{ EVENT_INPUT_BUTTON_UP_BACKWARD, this });
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_left)
					{
						Notify(new EventInfo{ EVENT_INPUT_BUTTON_UP_LEFT, this });
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_right)
					{
						Notify(new EventInfo{ EVENT_INPUT_BUTTON_UP_RIGHT, this });
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_up)
					{
						Notify(new EventInfo{ EVENT_INPUT_BUTTON_UP_UP, this });
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_down)
					{
						Notify(new EventInfo{ EVENT_INPUT_BUTTON_UP_DOWN, this });
					}

					// Quit button
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_quit)
					{
						Notify(new EventInfo{ EVENT_ENGINE_QUIT, this });
					}
				}
			}
			break;

			case SDL_MOUSEBUTTONDOWN:
			{
				/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEBUTTONDOWN detected!!!" });*/
				break;
			}			
			case SDL_MOUSEBUTTONUP:
			{
				/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEBUTTONUP detected!!!" });*/
				break;
			}			
			case SDL_MOUSEMOTION:
			{
				/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEMOTION detected!!!" });*/
				break;
			}
				

			default: // We don't care about anything else
			{
				break;
			}

			}// End switch
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