#include "InputManager.h"
#include "CoreEngine.h"

#include "SDL_keyboard.h"
#include "SDL_keycode.h"

//#include <string> // DEBUG


namespace BlazeEngine
{
	InputManager::InputManager() : EngineComponent()
	{
		SetName("InputManager");

		for (int i = 0; i < INPUT_NUM_BUTTON_INPUTS; i++)
		{
			buttonStates[i] = false;
		}

		for (int i = 0; i < INPUT_NUM_INPUT_AXIS; i++)
		{
			mouseAxisStates[i] = 0.0;
		}
	}

	//InputManager::~InputManager()
	//{
	//
	//}

	InputManager& InputManager::Instance()
	{
		static InputManager* instance = new InputManager();
		return *instance;
	}

	bool InputManager::GetInput(INPUT_BUTTON key)
	{
		return buttonStates[key];
	}

	double InputManager::GetMouseAxisInput(INPUT_AXIS axis)
	{
		return mouseAxisStates[axis];
	}
	
	void InputManager::Startup(CoreEngine* coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		this->coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Input manager started!" });
	}

	void InputManager::Shutdown()
	{
		coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Input manager shutting down..." });
	}

	void InputManager::Update()
	{
		const char* text; // DEBUG

		const Uint8* keyboardState;
		int numKeys;
		bool doFireEvent;

		SDL_Event currentSDLEvent;
		while (SDL_PollEvent(&currentSDLEvent))
		{
			doFireEvent = true;

			switch (currentSDLEvent.type) // We update button states on keydown/keyup
			{
			case SDL_KEYDOWN:

				numKeys = 0;
				keyboardState  = SDL_GetKeyboardState(&numKeys);
				if (numKeys > 0)
				{
					if (currentSDLEvent.key.keysym.sym == bindings.Button_forward)
					{
						buttonStates[INPUT_BUTTON_FORWARD] = true;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_backward)
					{
						buttonStates[INPUT_BUTTON_BACKWARD] = true;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_left)
					{
						buttonStates[INPUT_BUTTON_LEFT] = true;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_right)
					{
						buttonStates[INPUT_BUTTON_RIGHT] = true;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_up)
					{
						buttonStates[INPUT_BUTTON_UP] = true;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_down)
					{
						buttonStates[INPUT_BUTTON_DOWN] = true;
					}

					else if (currentSDLEvent.key.keysym.sym == bindings.Button_quit)
					{
						buttonStates[INPUT_BUTTON_QUIT] = true;
					}

					else // We weren't listening for this button: No need to fire an event
					{
						doFireEvent = false;
					}

					// Fire an event, if necessary:
					if (doFireEvent)
					{
						coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_INPUT_BUTTON_DOWN, this });
					}
				}
				break;

			case SDL_KEYUP:

				numKeys = 0;
				keyboardState = SDL_GetKeyboardState(&numKeys);
				if (numKeys > 0)
				{			
					if (currentSDLEvent.key.keysym.sym == bindings.Button_forward)
					{
						buttonStates[INPUT_BUTTON_FORWARD] = false;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_backward)
					{
						buttonStates[INPUT_BUTTON_BACKWARD] = false;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_left)
					{
						buttonStates[INPUT_BUTTON_LEFT] = false;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_right)
					{
						buttonStates[INPUT_BUTTON_RIGHT] = false;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_up)
					{
						buttonStates[INPUT_BUTTON_UP] = false;
					}
					else if (currentSDLEvent.key.keysym.sym == bindings.Button_down)
					{
						buttonStates[INPUT_BUTTON_DOWN] = false;
					}

					else if (currentSDLEvent.key.keysym.sym == bindings.Button_quit)
					{
						buttonStates[INPUT_BUTTON_QUIT] = false;
					}

					else // We weren't listening for this button: No need to fire an event
					{
						doFireEvent = false;
					}

					// Fire an event, if necessary:
					if (doFireEvent)
					{
						coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_INPUT_BUTTON_UP, this });
					}
				}
				break;

			// TO DO: IMPLEMENT MOUSE HANDLING...
			case SDL_MOUSEBUTTONDOWN:
				/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEBUTTONDOWN detected!!!" });*/
				break;

			case SDL_MOUSEBUTTONUP:
				/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEBUTTONUP detected!!!" });*/
				break;

			case SDL_MOUSEMOTION:
				/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEMOTION detected!!!" });*/
				break;

			default: // We don't care about anything else
				break;
			}
		}
	}
}

