// Allows features to be switched on/off between builds

#pragma once




// Flags:
#define DEBUG_LOG_OUTPUT			// Comment this out to exclude logging for release builds

#if defined(DEBUG_LOG_OUTPUT)

	// Event logging:
	//---------------
	//#define DEBUG_PRINT_NOTIFICATIONS				// Print notifications as the event manager receives them
	
	//#define DEBUG_LOGMANAGER_LOG_EVENTS				// Print events as the log manager receives them. Logged events are configured below
	#if defined (DEBUG_LOGMANAGER_LOG_EVENTS)
		#define DEBUG_LOGMANAGER_KEY_INPUT_LOGGING		// Log keypress input events
		#define DEBUG_LOGMANAGER_MOUSE_INPUT_LOGGING	// Log mouse input events
		#define DEBUG_LOGMANAGER_QUIT_LOGGING			// Log quit 
	#endif
	

	// OpenGL-specific logging (in RenderManager.cpp)
	//-----------------------------------------------
	#define DEBUG_LOG_OPENGL						// Enable/disable OpenGL logging
	#if defined(DEBUG_LOG_OPENGL)
		//#define DEBUG_LOG_OPENGL_NOTIFICATIONS	// Enable non-essential logging (e.g. OpenGL notifications)
	#endif
	


	// Uncomment to enable the Visual Studio "Visual Leak Detector" plugin
	// Get it here: https://marketplace.visualstudio.com/items?itemName=ArkadyShapkin.VisualLeakDetectorforVisualC
	//#include <vld.h>


	// Scene setup and creation logging:
	//----------------------------------
	#define DEBUG_SCENEMANAGER_LOG_SCENE_SETUP		// Enable/disable scene import logging
	#if defined(DEBUG_SCENEMANAGER_LOG_SCENE_SETUP)
		#define DEBUG_SCENEMANAGER_LIGHT_LOGGING	// Enable logging of light import/creation
	#endif


#endif // End of DEBUG_LOG_OUTPUT


// Log Manager static logging function definitions:
// ------------------------------------------------

#if defined (DEBUG_LOG_OUTPUT)
	#include "LogManager.h"
	#define LOG(message)			LogManager::Log(message);
	#define LOG_WARNING(message)	LogManager::LogWarning(message);
	#define LOG_ERROR(message)		LogManager::LogError(message);
#else
	#define LOG(message)			do {} while(false);
	#define LOG_WARNING(message)	do {} while(false);
	#define LOG_ERROR(message)		do {} while(false);
#endif