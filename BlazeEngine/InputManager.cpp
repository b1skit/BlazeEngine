#include "InputManager.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"

#include "SDL_keyboard.h"
#include "SDL_keycode.h"


namespace BlazeEngine
{
	// Static members:
	bool InputManager::buttonStates[INPUT_NUM_STATES];
	float InputManager::mouseAxisStates[INPUT_NUM_INPUT_AXIS];


	// Constructor:
	InputManager::InputManager() : EngineComponent("InputManager")
	{
		for (int i = 0; i < INPUT_NUM_STATES; i++)
		{
			buttonStates[i] = false;
		}

		for (int i = 0; i < INPUT_NUM_INPUT_AXIS; i++)
		{
			mouseAxisStates[i] = 0.0f;
		}
	}


	InputManager& InputManager::Instance()
	{
		static InputManager* instance = new InputManager();
		return *instance;
	}


	bool const& InputManager::GetInputState(INPUT_STATE key)
	{
		return InputManager::buttonStates[key];
	}


	float InputManager::GetMouseAxisInput(INPUT_AXIS axis)
	{
		float sensitivity;
		if (axis == INPUT_MOUSE_X)
		{
			sensitivity = CoreEngine::GetCoreEngine()->GetConfig()->input.mousePitchSensitivity;
		}
		else
		{
			sensitivity = CoreEngine::GetCoreEngine()->GetConfig()->input.mouseYawSensitivity;
		}

		return InputManager::mouseAxisStates[axis] * sensitivity;
	}
	

	void InputManager::Startup()
	{
		this->LoadInputBindings();

		LOG("Input manager started!");
	}


	void InputManager::Shutdown()
	{
		LOG("Input manager shutting down...");
	}


	void InputManager::Update()
	{
		// Get the mouse deltas, once per frame:
		int xRel, yRel = 0;
		SDL_GetRelativeMouseState(&xRel, &yRel);
		mouseAxisStates[INPUT_MOUSE_X] = (float)xRel;
		mouseAxisStates[INPUT_MOUSE_Y] = (float)yRel;

		// Mouse button states:
		buttonStates[INPUT_MOUSE_LEFT] = (bool)(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT));
		buttonStates[INPUT_MOUSE_RIGHT] = (bool)(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT));

		// Update keystates:
		const Uint8* state = SDL_GetKeyboardState(NULL);

		buttonStates[INPUT_BUTTON_FORWARD]	= (bool)state[this->inputBindings[INPUT_BUTTON_FORWARD]];
		buttonStates[INPUT_BUTTON_BACKWARD] = (bool)state[this->inputBindings[INPUT_BUTTON_BACKWARD]];
		buttonStates[INPUT_BUTTON_LEFT]		= (bool)state[this->inputBindings[INPUT_BUTTON_LEFT]];
		buttonStates[INPUT_BUTTON_RIGHT]	= (bool)state[this->inputBindings[INPUT_BUTTON_RIGHT]];
		buttonStates[INPUT_BUTTON_UP]		= (bool)state[this->inputBindings[INPUT_BUTTON_UP]];
		buttonStates[INPUT_BUTTON_DOWN]		= (bool)state[this->inputBindings[INPUT_BUTTON_DOWN]];

		buttonStates[INPUT_BUTTON_QUIT]		= (bool)state[this->inputBindings[INPUT_BUTTON_QUIT]];
	}


	void InputManager::HandleEvent(EventInfo const * eventInfo)
	{
		
	}


	void InputManager::LoadInputBindings()
	{
		// TODO: Load from a file. For now, we just hard code

		// Initialize key mappings:
		this->inputBindings[INPUT_BUTTON_FORWARD]	= SDL_SCANCODE_W;
		this->inputBindings[INPUT_BUTTON_BACKWARD]	= SDL_SCANCODE_S;
		this->inputBindings[INPUT_BUTTON_LEFT]		= SDL_SCANCODE_A;
		this->inputBindings[INPUT_BUTTON_RIGHT]		= SDL_SCANCODE_D;
		this->inputBindings[INPUT_BUTTON_UP]		= SDL_SCANCODE_SPACE;
		this->inputBindings[INPUT_BUTTON_DOWN]		= SDL_SCANCODE_LSHIFT;
		
		this->inputBindings[INPUT_BUTTON_QUIT]		= SDL_SCANCODE_ESCAPE;

		// Initialize mouse mappings as 0 (== SDL_SCANCODE_UNKNOWN)
		this->inputBindings[INPUT_MOUSE_LEFT]	= SDL_SCANCODE_UNKNOWN;
		this->inputBindings[INPUT_MOUSE_RIGHT]	= SDL_SCANCODE_UNKNOWN;
	}
}

