// Input and control manager.
// Catches SDL events for BlazeEngine


#pragma once
#include "EngineComponent.h"
#include "SDL_events.h"

namespace BlazeEngine
{
	// Binary controls
	enum INPUT_BUTTON
	{		
		INPUT_BUTTON_FORWARD,
		INPUT_BUTTON_BACKWARD,
		INPUT_BUTTON_LEFT,
		INPUT_BUTTON_RIGHT,
		INPUT_BUTTON_UP,
		INPUT_BUTTON_DOWN,

		INPUT_MOUSE_LEFT,
		INPUT_MOUSE_RIGHT,

		INPUT_BUTTON_QUIT, // Temporary: Hard code a quit button

		INPUT_NUM_BUTTON_INPUTS // RESERVED: A count of the number of INPUT_BUTTON's
	};

	// Analogue controls (eg. mouse movement)
	enum INPUT_AXIS
	{
		INPUT_MOUSE_X,
		INPUT_MOUSE_Y,
		
		INPUT_NUM_INPUT_AXIS	// RESERVED: A count of the number of INPUT_AXIS's
	};

	class InputManager : public EngineComponent
	{
	public:
		InputManager();
		/*~InputManager();*/

		// Singleton functionality:
		static InputManager& Instance();
		InputManager(InputManager const&) = delete; // Disallow copying of our Singleton
		void operator=(InputManager const&) = delete;

		// Member functions:
		int GetInput(INPUT_BUTTON button);
		double GetMouseAxisInput(INPUT_AXIS axis);


		// EngineComponent interface:
		void Startup(CoreEngine* coreEngine);

		void Shutdown();

		void Update();

	private:
		bool buttonStates[INPUT_NUM_BUTTON_INPUTS]; // Stores the state of buttons. Updated on key down/up
		double mouseAxisStates[INPUT_NUM_INPUT_AXIS];

		// Control configuration
		// TO DO: Break this out into an external file...
		
		SDL_Keycode Button_forward = SDLK_w;
		SDL_Keycode Button_backward = SDLK_s;
		SDL_Keycode Button_left = SDLK_a;
		SDL_Keycode Button_right = SDLK_d;
		SDL_Keycode Button_up = SDLK_SPACE;
		SDL_Keycode Button_down = SDLK_LSHIFT;

		unsigned short Button_leftClick = SDL_BUTTON_LEFT;
		unsigned short Button_rightClick = SDL_BUTTON_RIGHT;

		// TO DO: Bind mouse axis's

		SDL_Keycode Button_quit = SDLK_ESCAPE;
	};
}


