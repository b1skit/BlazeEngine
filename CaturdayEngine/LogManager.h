#pragma once
#include "EventGenerator.h"
#include "EventListener.h"

namespace CaturdayEngine
{


	class LogManager : EventGenerator, EventListener
	{
	public:
		static LogManager& Instance()
		{
			static LogManager* instance = new LogManager();
			return *instance;
		}

		int HandleEvent(EVENT_TYPE event, EventGenerator* generator);

		//LogManager();
		//~LogManager();

	private:
		
		
	};

}