#include "InputManager.h"
#include "CoreEngine.h"

#include "SDL_keyboard.h"
#include "SDL_keycode.h"



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
		SDL_Event currentSDLEvent;
		while (SDL_PollEvent(&currentSDLEvent))
		{
			
			switch (currentSDLEvent.type)
			{
			case SDL_TEXTINPUT:
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Text input detected!!!" });
				break;

			case SDL_MOUSEMOTION:
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Mouse motion detected!!!" });
				break;

			default:
				coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_ERROR, this, "Unhandled input event!" });
				break;
			}
		}
	}
}

