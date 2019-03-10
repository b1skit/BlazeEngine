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

		// Get the time elapsed since the last frame, in seconds
		inline double GetDeltaTimeSeconds()
		{
			return (double)GetDeltaTimeMs() * 0.001; // Convert: ms->sec
		}

		// Get the time elapsed since the last frame, in milliseconds
		inline unsigned int GetDeltaTimeMs()
		{
			return deltaTime;
		}

		inline unsigned int GetTotalRunningTimeMs()
		{
			return currentTime - startTime;
		}

		inline double GetTotalRunningTimeSeconds()
		{
			return (double)GetTotalRunningTimeMs() * 0.001;
		}


	protected:


	private:
		unsigned int startTime;
		unsigned int prevTime;
		unsigned int currentTime;
		unsigned int deltaTime;

		/*double timeScale;*/
	};
}


