// Input and control manager.
// Catches SDL events for BlazeEngine


#pragma once
#include "EngineComponent.h"
#include "SDL_events.h"
#include "EventListener.h"

namespace BlazeEngine
{
	// Binary controls: Enums index buttonStates array elements
	enum INPUT_STATE
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

		INPUT_NUM_STATES // RESERVED: A count of the number of INPUT_STATE's
	};

	// Analogue controls (eg. mouse movement): Enums index mouseAxisStates array elements
	enum INPUT_AXIS
	{
		INPUT_MOUSE_X,
		INPUT_MOUSE_Y,
		
		INPUT_NUM_INPUT_AXIS	// RESERVED: A count of the number of INPUT_AXIS's
	};


	class InputManager : public EngineComponent, public EventListener
	{
	public:
		InputManager();

		// Singleton functionality:
		static InputManager& Instance();
		InputManager(InputManager const&)	= delete; // Disallow copying of our Singleton
		void operator=(InputManager const&) = delete;

		// Static member functions:
		static bool const&	GetInputState(INPUT_STATE button);
		static float		GetMouseAxisInput(INPUT_AXIS axis);

		// EngineComponent interface:
		void Startup();
		void Shutdown();
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		void LoadInputBindings();

	private:
		int				inputBindings[INPUT_NUM_STATES];		// Stores mapping from INPUT_STATE enums to SDL_SCANCODE_ values

		static bool		buttonStates[INPUT_NUM_STATES];			// Stores the state of buttons
		static float	mouseAxisStates[INPUT_NUM_INPUT_AXIS];	// Mouse axis deltas

		// Cache sensitivity params:
		static float mousePitchSensitivity;
		static float mouseYawSensitivity;
	};
}


