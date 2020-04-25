#include "InputManager.h"
#include "CoreEngine.h"
#include "BuildConfiguration.h"


namespace BlazeEngine
{
	// Static members:
	bool InputManager::keyboardButtonStates[INPUT_NUM_BUTTONS];
	bool InputManager::mouseButtonStates[INPUT_MOUSE_NUM_BUTTONS];
	float InputManager::mouseAxisStates[INPUT_NUM_INPUT_AXIS];

	float InputManager::mousePitchSensitivity	= -0.00005f;
	float InputManager::mouseYawSensitivity		= -0.00005f;


	// Constructor:
	InputManager::InputManager() : EngineComponent("InputManager")
	{
		// Initialize keyboard keys:
		for (int i = 0; i < INPUT_NUM_BUTTONS; i++)
		{
			inputKeyboardBindings[i]		= SDL_SCANCODE_UNKNOWN; // == 0
			keyboardButtonStates[i]			= false;
		}

		// Initialize mouse axes:
		for (int i = 0; i < INPUT_NUM_INPUT_AXIS; i++)
		{
			mouseAxisStates[i]	= 0.0f;
		}
	}


	InputManager& InputManager::Instance()
	{
		static InputManager* instance = new InputManager();
		return *instance;
	}


	bool const& InputManager::GetKeyboardInputState(KEYBOARD_BUTTON_STATE key)
	{
		return InputManager::keyboardButtonStates[key];
	}


	bool const& InputManager::GetMouseInputState(MOUSE_BUTTON_STATE button)
	{
		return InputManager::mouseButtonStates[button];
	}


	float InputManager::GetMouseAxisInput(INPUT_AXIS axis)
	{
		float sensitivity;
		if (axis == INPUT_MOUSE_X)
		{
			sensitivity = mousePitchSensitivity;
		}
		else
		{
			sensitivity = mouseYawSensitivity;
		}

		return InputManager::mouseAxisStates[axis] * sensitivity;
	}
	

	void InputManager::Startup()
	{
		LOG("InputManager starting...");

		this->LoadInputBindings();

		// Cache sensitivity params:
		InputManager::mousePitchSensitivity	= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<float>("mousePitchSensitivity");
		InputManager::mouseYawSensitivity	= CoreEngine::GetCoreEngine()->GetConfig()->GetValue<float>("mouseYawSensitivity");
	}


	void InputManager::Shutdown()
	{
		LOG("Input manager shutting down...");
	}


	void InputManager::Update()
	{
		// Update keyboard states:
		const Uint8* SDLKeyboardState = SDL_GetKeyboardState(NULL);

		keyboardButtonStates[INPUT_BUTTON_FORWARD]	= (bool)SDLKeyboardState[this->inputKeyboardBindings[INPUT_BUTTON_FORWARD]];
		keyboardButtonStates[INPUT_BUTTON_BACKWARD] = (bool)SDLKeyboardState[this->inputKeyboardBindings[INPUT_BUTTON_BACKWARD]];
		keyboardButtonStates[INPUT_BUTTON_LEFT]		= (bool)SDLKeyboardState[this->inputKeyboardBindings[INPUT_BUTTON_LEFT]];
		keyboardButtonStates[INPUT_BUTTON_RIGHT]	= (bool)SDLKeyboardState[this->inputKeyboardBindings[INPUT_BUTTON_RIGHT]];
		keyboardButtonStates[INPUT_BUTTON_UP]		= (bool)SDLKeyboardState[this->inputKeyboardBindings[INPUT_BUTTON_UP]];
		keyboardButtonStates[INPUT_BUTTON_DOWN]		= (bool)SDLKeyboardState[this->inputKeyboardBindings[INPUT_BUTTON_DOWN]];

		keyboardButtonStates[INPUT_BUTTON_QUIT]		= (bool)SDLKeyboardState[this->inputKeyboardBindings[INPUT_BUTTON_QUIT]];


		// Update mouse button states:
		mouseButtonStates[INPUT_MOUSE_LEFT]			= (bool)(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT));
		mouseButtonStates[INPUT_MOUSE_RIGHT]		= (bool)(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT));


		// Get the mouse deltas, once per frame:
		int xRel, yRel = 0;
		SDL_GetRelativeMouseState(&xRel, &yRel);
		mouseAxisStates[INPUT_MOUSE_X] = (float)xRel;
		mouseAxisStates[INPUT_MOUSE_Y] = (float)yRel;
	}


	void InputManager::HandleEvent(EventInfo const * eventInfo)
	{
		
	}


	void InputManager::LoadInputBindings()
	{
		for (int i = 0; i < INPUT_NUM_BUTTONS; i++)
		{
			SDL_Scancode theScancode;

			string buttonString = CoreEngine::GetCoreEngine()->GetConfig()->GetValueAsString(KEY_NAMES[i]);

			// Handle chars:
			if (buttonString.length() == 1)
			{
				theScancode = SDL_GetScancodeFromKey((SDL_Keycode)buttonString.c_str()[0]);
			}
			// Handle strings:
			else
			{
				auto result = SCANCODE_MAPPINGS.find(buttonString);
				if (result != SCANCODE_MAPPINGS.end())
				{
					theScancode = result->second;
				}
			}

			this->inputKeyboardBindings[i] = theScancode;
		}
	}
}

