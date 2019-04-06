// Handles logging for the engine and 

#include "EventManager.h"
#include "BlazeObject.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#include "SDL.h"

// DEBUG: Spew notification messages as they're recieved.
// Useful for debugging when an issue occurs before we can print it normally
//#define DEBUG_PRINT_NOTIFICATIONS
//#if defined(DEBUG_PRINT_NOTIFICATIONS)
//	#include <string>
//	#include <iostream>
//	using std::string;
//	using std::cout;
//#endif


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
		bool doFireEvent = true;

		// Catch relevant events from SDL2:
		SDL_Event currentSDLEvent;
		while (SDL_PollEvent(&currentSDLEvent))
		{
			switch (currentSDLEvent.type)
			{
			case SDL_QUIT:
			{
				Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, new string("Received SDL_QUIT event") });
				break;
			}

			case SDL_KEYDOWN:
			{
				int numKeys = 0;
				const Uint8* keyboardState = SDL_GetKeyboardState(&numKeys);
				InputBindings const* inputBindings = CoreEngine::GetInputManager()->GetInputBindings();
				if (numKeys > 0)
				{
					// Button input events:
					if (currentSDLEvent.key.keysym.sym == inputBindings->Button_forward)
					{
						if (!CoreEngine::GetInputManager()->GetInputState(INPUT_BUTTON_FORWARD))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_FORWARD, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_backward)
					{
						if (!CoreEngine::GetInputManager()->GetInputState(INPUT_BUTTON_BACKWARD))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_BACKWARD, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_left)
					{
						if (!CoreEngine::GetInputManager()->GetInputState(INPUT_BUTTON_LEFT))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_LEFT, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_right)
					{
						if (!CoreEngine::GetInputManager()->GetInputState(INPUT_BUTTON_RIGHT))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_RIGHT, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_up)
					{
						if (!CoreEngine::GetInputManager()->GetInputState(INPUT_BUTTON_UP))
						{
							Notify(new EventInfo{ EVENT_INPUT_BUTTON_DOWN_UP, this });
						}
					}
					else if (currentSDLEvent.key.keysym.sym == inputBindings->Button_down)
					{
						if (!CoreEngine::GetInputManager()->GetInputState(INPUT_BUTTON_DOWN))
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
				InputBindings const* inputBindings = CoreEngine::GetInputManager()->GetInputBindings();
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
				switch (currentSDLEvent.button.button)
				{
				case SDL_BUTTON_LEFT:
					Notify(new EventInfo{ EVENT_INPUT_MOUSE_CLICK_LEFT, this });
					break;

				case SDL_BUTTON_RIGHT:
					Notify(new EventInfo{ EVENT_INPUT_MOUSE_CLICK_RIGHT, this });
					break;
				}				
				
				break;
			}			
			case SDL_MOUSEBUTTONUP:
			{
				switch (currentSDLEvent.button.button)
				{
				case SDL_BUTTON_LEFT:
					Notify(new EventInfo{ EVENT_INPUT_MOUSE_RELEASE_LEFT, this });
					break;

				case SDL_BUTTON_RIGHT:
					Notify(new EventInfo{ EVENT_INPUT_MOUSE_RELEASE_RIGHT, this });
					break;
				}

				break;
			}			
			case SDL_MOUSEMOTION:
			{
				Notify(new EventInfo{ EVENT_INPUT_MOUSE_MOVED, this, nullptr });				
				break;
			}
				

			default: // We don't care about anything else
			{
				break;
			}

			}// End switch
		}

		// Catch OpenGl error events:
		GLenum glError;
		string prefix = "OpenGL error: ";
		while ((glError = glGetError()) != GL_NO_ERROR) // TO DO: REPLACE THIS WITH AN OPENGL ERROR HANDLING CALLBACK FUNCTION
		{
			switch (glError)
			{
			case GL_INVALID_ENUM:
				LOG_ERROR(prefix + "GL_INVALID_ENUM");
				break;

			case GL_INVALID_VALUE:
				LOG_ERROR(prefix + "GL_INVALID_VALUE");
				break;

			case GL_INVALID_OPERATION:
				LOG_ERROR(prefix + "GL_INVALID_OPERATION");
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				LOG_ERROR(prefix + "GL_INVALID_FRAMEBUFFER_OPERATION");
				break;

			case GL_OUT_OF_MEMORY:
				LOG_ERROR(prefix + "GL_OUT_OF_MEMORY");
				break;

			case GL_STACK_UNDERFLOW:
				LOG_ERROR(prefix + "GL_OUT_OF_MEMORY");
				break;

			case GL_STACK_OVERFLOW:
				LOG_ERROR(prefix + "GL_STACK_OVERFLOW");
				break;

			default:
				LOG_ERROR(prefix + to_string(glError));
				break;
			}
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

	void EventManager::Notify(EventInfo const* eventInfo, bool pushToFront)
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

		// Selec what to notify based on type?

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