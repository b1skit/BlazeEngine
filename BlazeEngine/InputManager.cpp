#include "InputManager.h"
#include "CoreEngine.h"

#include "SDL_keyboard.h"
#include "SDL_keycode.h"

#include <string> // DEBUG


namespace BlazeEngine
{
	InputManager::InputManager() : EngineComponent()
	{
		SetName("InputManager");

		for (int i = 0; i < INPUT_NUM_KEY_INPUTS; i++)
		{
			inputKeys[i] = 0;
		}

		for (int i = 0; i < INPUT_NUM_MOUSE_INPUTS; i++)
		{
			inputMouse[i] = 0.0;
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

	int InputManager::GetInput(INPUT_KEY key)
	{
		return (int)inputKeys[(int)key];
	}

	double InputManager::GetInput(INPUT_ANALOGUE axis)
	{
		return inputMouse[(int)axis];
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

		SDL_Event currentSDLEvent;
		while (SDL_PollEvent(&currentSDLEvent))
		{
			
			switch (currentSDLEvent.type)
			{
			case SDL_TEXTINPUT:
				text = currentSDLEvent.text.text;
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_TEXTINPUT detected!!! " + string( text)  }); 

				break;

			case SDL_KEYDOWN:
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_KEYDOWN detected!!!" });

				numKeys = 0;
				keyboardState  = SDL_GetKeyboardState(&numKeys);
				if (numKeys > 0)
				{
					if (keyboardState[Button_quit])
					{
						coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_ENGINE_QUIT, this, "InputManager generated EVENT_ENGINE_QUIT..." });
					}
				}

				break;

			case SDL_KEYUP:
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_KEYUP detected!!!" });
				break;

			case SDL_MOUSEMOTION:
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEMOTION detected!!!" });
				break;

			case SDL_MOUSEBUTTONDOWN:
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "SDL_MOUSEBUTTONDOWN detected!!!" });
				break;

			default:
				/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_ERROR, this, "Error! Unhandled input event!" });*/
				break;
			}
		}
	}
}

