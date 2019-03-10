// Time manager class
// Responsible for tracking all time-related info

#pragma once

#include "EngineComponent.h"
#include "Dependencies/SDL2/include/SDL_timer.h"

namespace BlazeEngine
{
	class TimeManager : EngineComponent
	{
	public:
		TimeManager();
		/*~TimeManager();*/

		// Singleton functionality:
		static TimeManager& Instance();
		TimeManager(TimeManager const&) = delete; // Disallow copying of our Singleton
		void operator=(TimeManager const&) = delete;
		
		// EngineComponent interface:
		void Startup(CoreEngine* coreEngine);

		void Shutdown();

		void Update();

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
		static inline double DeltaTime()
		{
			return deltaTime;
		}

		static unsigned int GetTotalRunningTimeMs()
		{
			return currentTime - startTime;
		}

		static double GetTotalRunningTimeSeconds()
		{
			return (double)GetTotalRunningTimeMs() * 0.001;
		}

		
	protected:


	private:
		static unsigned int startTime;
		static unsigned int prevTime;
		static unsigned int currentTime;
		static double deltaTime;

		

		/*double timeScale;*/
	};

	
}


