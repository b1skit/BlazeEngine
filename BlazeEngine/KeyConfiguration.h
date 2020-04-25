// Input key string defines. Used to share mappings between config keys/values and input managers

#pragma once

#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "SDL_scancode.h"

#include <unordered_map>
#include <string>

using std::string;
using std::unordered_map;


// Macros:
#define MACRO_TO_STR(x) #x

// Default true/false strings (Must be lowercase)
#define TRUE_STRING				"true"		
#define FALSE_STRING			"false"

// Command strings: End with a space to maintain formatting
#define SET_CMD					"set "		// Set a value
#define BIND_CMD				"bind "		// Bind a key


// Input binding names: Used as hash keys in EngineConfig <key, value> mappings
#define INPUT_BUTTON_FORWARD	btn_forward
#define INPUT_BUTTON_BACKWARD	btn_backward
#define INPUT_BUTTON_LEFT		btn_strafeLeft
#define INPUT_BUTTON_RIGHT		btn_strafeRight
#define INPUT_BUTTON_UP			btn_up
#define INPUT_BUTTON_DOWN		btn_down

#define INPUT_BUTTON_QUIT		btn_quit

#define INPUT_MOUSE_LEFT		mouse_left
#define INPUT_MOUSE_RIGHT		mouse_right


// Key binding names: Used as hash key names in InputManager to map keys to SDL ScanCodes
#define L_SHIFT					"lshift"
#define SPACE					"space"
#define ESC						"esc"


namespace BlazeEngine
{
	const unordered_map<string, SDL_Scancode> SCANCODE_MAPPINGS =
	{
		{L_SHIFT,				SDL_SCANCODE_LSHIFT},
		{SPACE,					SDL_SCANCODE_SPACE},
		{ESC,					SDL_SCANCODE_ESCAPE},
	};


	// Binary controls: Enums index keyboardButtonStates array elements
	enum KEYBOARD_BUTTON_STATE
	{
		INPUT_BUTTON_FORWARD,
		INPUT_BUTTON_BACKWARD,
		INPUT_BUTTON_LEFT,
		INPUT_BUTTON_RIGHT,
		INPUT_BUTTON_UP,
		INPUT_BUTTON_DOWN,

		INPUT_BUTTON_QUIT, // Temporary: Hard code a quit button

		INPUT_NUM_BUTTONS // RESERVED: A count of the number of INPUT_STATEs
	};

	// Array of key name strings: Used to iterate through all possible buttons
	// Note: These MUST be in the same order as the KEYBOARD_BUTTON_STATE enum
	const string KEY_NAMES[INPUT_NUM_BUTTONS] =
	{
		MACRO_TO_STR(INPUT_BUTTON_FORWARD),
		MACRO_TO_STR(INPUT_BUTTON_BACKWARD),
		MACRO_TO_STR(INPUT_BUTTON_LEFT),
		MACRO_TO_STR(INPUT_BUTTON_RIGHT),
		MACRO_TO_STR(INPUT_BUTTON_UP),
		MACRO_TO_STR(INPUT_BUTTON_DOWN),

		MACRO_TO_STR(INPUT_BUTTON_QUIT),
	};

	enum MOUSE_BUTTON_STATE
	{
		INPUT_MOUSE_LEFT,
		INPUT_MOUSE_RIGHT,

		INPUT_MOUSE_NUM_BUTTONS // RESERVED: A count of the number of MOUSE_BUTTON_STATEs
	};

	// Array of mouse button name strings: Used to iterate through all possible buttons
	// Note: These MUST be in the same order as the MOUSE_BUTTON_STATE enum
	const string MOUSE_BUTTON_NAMES[INPUT_MOUSE_NUM_BUTTONS] =
	{
		MACRO_TO_STR(INPUT_MOUSE_LEFT),
		MACRO_TO_STR(INPUT_MOUSE_RIGHT),
	};

	// Analogue controls (eg. mouse movement): Enums index mouseAxisStates array elements
	enum INPUT_AXIS
	{
		INPUT_MOUSE_X,
		INPUT_MOUSE_Y,

		INPUT_NUM_INPUT_AXIS	// RESERVED: A count of the number of INPUT_AXIS's
	};	
}