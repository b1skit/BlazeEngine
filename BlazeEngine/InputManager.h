// Input and control manager.
// Catches SDL events for BlazeEngine


#pragma once
#include "EngineComponent.h"
#include "SDL_events.h"

namespace BlazeEngine
{
	// Binary controls
	enum INPUT_KEY
	{		
		INPUT_KEY_FORWARD,
		INPUT_KEY_BACKWARD,
		INPUT_KEY_LEFT,
		INPUT_KEY_RIGHT,

		INPUT_NUM_KEY_INPUTS // RESERVED: A count of the number of INPUT_KEY's
	};

	// Analogue controls (eg. mouse movement)
	enum INPUT_ANALOGUE
	{
		INPUT_MOUSE_X,
		INPUT_MOUSE_Y,

		INPUT_NUM_MOUSE_INPUTS	// RESERVED: A count of the number of INPUT_MOUSE's
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
		int GetInput(INPUT_KEY key);
		double GetInput(INPUT_ANALOGUE axis);


		// EngineComponent interface:
		void Startup(CoreEngine* coreEngine);

		void Shutdown();

		void Update();

	private:
		short int inputKeys[INPUT_NUM_KEY_INPUTS];
		double inputMouse[INPUT_NUM_MOUSE_INPUTS];

		// Control configuration
		// TO DO: Break this out into an external file...

		SDL_Scancode Button_quit = SDL_SCANCODE_ESCAPE;

	};
}


