#include "InputManager.h"
#include "CoreEngine.h"

#include "SDL_keyboard.h"
#include "SDL_keycode.h"

//#include <string> // DEBUG


namespace BlazeEngine
{
	// Static members:
	bool InputManager::buttonStates[INPUT_NUM_STATES];
	double InputManager::mouseAxisStates[INPUT_NUM_INPUT_AXIS];


	// Constructor:
	InputManager::InputManager() : EngineComponent("InputManager")
	{
		for (int i = 0; i < INPUT_NUM_STATES; i++)
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

	bool InputManager::GetInputState(INPUT_STATE key)
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

		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_BACKWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_DOWN, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_FORWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_RIGHT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_UP, this);

		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_BACKWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_DOWN, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_FORWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_RIGHT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_UP, this);

		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_CLICK_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_CLICK_RIGHT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_MOVED, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_RELEASE_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_RELEASE_RIGHT, this);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Input manager started!" });
	}

	void InputManager::Shutdown()
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Input manager shutting down..." });
	}

	void InputManager::Update()
	{
		buttonStates[INPUT_MOUSE_AXIS] = false;
		for (int i = 0; i < INPUT_NUM_INPUT_AXIS; i++)
		{
			mouseAxisStates[i] = 0.0;
		}
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

		case EVENT_INPUT_MOUSE_MOVED:
		{
			buttonStates[INPUT_MOUSE_AXIS] = true;
			mouseAxisStates[INPUT_MOUSE_X] = eventInfo->SDL_event->motion.xrel;
			mouseAxisStates[INPUT_MOUSE_Y] = eventInfo->SDL_event->motion.yrel;
		}	
		break;

		case EVENT_INPUT_MOUSE_CLICK_LEFT:
		case EVENT_INPUT_MOUSE_RELEASE_LEFT:
		{
			buttonStates[INPUT_MOUSE_LEFT] = !buttonStates[INPUT_MOUSE_LEFT];
		}
		break;

		case EVENT_INPUT_MOUSE_CLICK_RIGHT:
		case EVENT_INPUT_MOUSE_RELEASE_RIGHT:
		{
			buttonStates[INPUT_MOUSE_RIGHT] = !buttonStates[INPUT_MOUSE_RIGHT];
		}
		break;

		default:
			coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "ERROR: Default event generated in InputManager!" });
			break;
		}
	}
}

