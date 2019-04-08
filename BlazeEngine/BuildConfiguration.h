// Allows features to be switched on/off between builds

#pragma once

// Flags:
#define DEBUG_LOG_OUTPUT			// Comment this out to exclude logging for release builds

#if defined(DEBUG_LOG_OUTPUT)
	// Uncomment these for additional debug output settings:
	//------------------------------------------------------
	//#define DEBUG_PRINT_NOTIFICATIONS			// Print notifications as the event manager receives them
	
	//#define DEBUG_LOG_MANAGER_PRINT_EVENTS		// Print events as the log manager receives them

	
	// Logging verbosity: Which events the log manager will subscribe to:
	//-------------------------------------------------------------------
	//#define LOG_VERBOSITY_ALL
	//#define LOG_VERBOSITY_DEBUG
	// ^^ Currently unused...

	// OpenGL-specific logging (in RenderManager.cpp)
	#define DEBUG_LOG_OPENGL	// Comment this out to disable OpenGL logging

#endif


// Debug logging:
// --------------

#ifdef DEBUG_LOG_OUTPUT
	#include "LogManager.h"
	#define LOG(m) LogManager::Log(m);
	#define LOG_ERROR(m) LogManager::LogError(m);
#else
	#define LOG(message)		do {} while(false);
	#define LOG_ERROR(message)	do {} while(false);
#endif

