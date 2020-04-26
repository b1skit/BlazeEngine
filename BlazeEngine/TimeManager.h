// Time manager class
// Responsible for tracking all time-related info

#pragma once

#include "EngineComponent.h"	// Base class

#include "Dependencies/SDL2/include/SDL_timer.h"


namespace BlazeEngine
{
	class TimeManager : EngineComponent
	{
	public:
		TimeManager();

		// Singleton functionality:
		static TimeManager& Instance();
		TimeManager(TimeManager const&) = delete; // Disallow copying of our Singleton
		void operator=(TimeManager const&) = delete;
		
		// EngineComponent interface:
		void Startup();
		void Shutdown();
		void Update();
		void Destroy() {}	// Do nothing, for now...

		// Member functions:

		//inline unsigned int GetCurrentTime()
		//{
		//	return currentTime;
		//}

		//// Get the time elapsed since the last frame, in seconds
		//static double GetDeltaTimeSeconds()
		//{
		//	return (double)DeltaTime() * 0.001; // Convert: ms->sec
		//}

		// Get the time elapsed since the last frame, in ms
		static inline unsigned int	DeltaTime()						{ return deltaTime; }
		static unsigned int			GetTotalRunningTimeMs()			{ return currentTime - startTime; }
		static double				GetTotalRunningTimeSeconds()	{ return (double)GetTotalRunningTimeMs() * 0.001; }

		
	protected:


	private:
		static unsigned int startTime;
		static unsigned int prevTime;
		static unsigned int currentTime;
		static unsigned int	deltaTime;

		/*double timeScale;*/
	};

	
}


