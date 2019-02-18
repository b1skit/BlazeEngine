#include "InputManager.h"
#include "CoreEngine.h"

#include "SDL_keyboard.h"
#include "SDL_keycode.h"

//#include <string> // DEBUG


namespace BlazeEngine
{
	InputManager::InputManager() : EngineComponent("InputManager")
	{
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


		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Input manager started!" });
	}

	void InputManager::Shutdown()
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Input manager shutting down..." });
	}

	void InputManager::Update()
	{
		
	}

	void InputManager::HandleEvent(EventInfo const * eventInfo)
	{
		switch (eventInfo->type)
		{
		case EVENT_INPUT_BUTTON_DOWN_FORWARD:
		case EVENT_INPUT_BUTTON_UP_FORWARD:
		{
			buttonStates[INPUT_BUTTON_FORWARD] = !buttonStates[INPUT_BUTTON_FORWARD];
		}
		break;

		case EVENT_INPUT_BUTTON_DOWN_BACKWARD:
		case EVENT_INPUT_BUTTON_UP_BACKWARD:
		{
			buttonStates[INPUT_BUTTON_BACKWARD] = !buttonStates[INPUT_BUTTON_BACKWARD];
		}
		break;

		case EVENT_INPUT_BUTTON_DOWN_LEFT:
		case EVENT_INPUT_BUTTON_UP_LEFT:
		{
			buttonStates[INPUT_BUTTON_LEFT] = !buttonStates[INPUT_BUTTON_LEFT];
		}
		break;

		case EVENT_INPUT_BUTTON_DOWN_RIGHT:
		case EVENT_INPUT_BUTTON_UP_RIGHT: 
		{
			buttonStates[INPUT_BUTTON_RIGHT] = !buttonStates[INPUT_BUTTON_RIGHT];
		}
		break;

		case EVENT_INPUT_BUTTON_DOWN_UP:
		case EVENT_INPUT_BUTTON_UP_UP:
		{
			buttonStates[INPUT_BUTTON_UP] = !buttonStates[INPUT_BUTTON_UP];
		}
		break;

		case EVENT_INPUT_BUTTON_DOWN_DOWN:
		case EVENT_INPUT_BUTTON_UP_DOWN:
		{
			buttonStates[INPUT_BUTTON_DOWN] = !buttonStates[INPUT_BUTTON_DOWN];
		}
		break;


		default:
			coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "ERROR: Default event generated in InputManager!" });
			break;
		}
	}
}

