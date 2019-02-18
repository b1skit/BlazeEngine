// Input and control manager.
// Catches SDL events for BlazeEngine


#pragma once
#include "EngineComponent.h"
#include "SDL_events.h"
#include "EventListener.h"

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

	// Control configuration:
	struct InputBindings
	{
		// TO DO: Break this out into an external file...

		Sint32 Button_forward = SDLK_w;			// SDL_Keycodes, cast as signed 32 bit ints
		Sint32 Button_backward = SDLK_s;
		Sint32 Button_left = SDLK_a;
		Sint32 Button_right = SDLK_d;
		Sint32 Button_up = SDLK_SPACE;
		Sint32 Button_down = SDLK_LSHIFT;

		unsigned short Button_leftClick = SDL_BUTTON_LEFT;
		unsigned short Button_rightClick = SDL_BUTTON_RIGHT;

		// TO DO: Bind mouse axis's

		Sint32 Button_quit = SDLK_ESCAPE;
	};


	class InputManager : public EngineComponent, public EventListener
	{
	public:
		InputManager();
		/*~InputManager();*/

		// Singleton functionality:
		static InputManager& Instance();
		InputManager(InputManager const&) = delete; // Disallow copying of our Singleton
		void operator=(InputManager const&) = delete;

		// Member functions:
		bool GetInput(INPUT_BUTTON button);
		double GetMouseAxisInput(INPUT_AXIS axis);
		
		inline InputBindings const* GetInputBindings()
		{
			return &inputBindings;
		}

		// EngineComponent interface:
		void Startup(CoreEngine* coreEngine);

		void Shutdown();

		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

	private:
		bool buttonStates[INPUT_NUM_BUTTON_INPUTS]; // Stores the state of buttons. Updated on key down/up
		double mouseAxisStates[INPUT_NUM_INPUT_AXIS];

		// SDL2 event queue handling:
		const static int MAX_EVENTS = 10; // Max number of events to look for
		SDL_Event SDLEventBuffer[MAX_EVENTS];

		InputBindings inputBindings;
	};
}


