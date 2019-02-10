// Time manager class
// Responsible for tracking all time-related info

#pragma once

#include "EngineComponent.h"
#include <time.h>

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
		inline double GetCurrentTime()
		{
			return currentTime;
		}

		// Get the time elapsed since the last frame
		inline double GetDeltaTime()
		{

			return 0.0f; // TEMP
		}


	protected:


	private:
		double startTime;
		double currentTime;
		double lastFrameTime;
		double currentFrameTime;
		double deltaTime;
	};
}


