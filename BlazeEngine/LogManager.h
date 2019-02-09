#pragma once
#include "EventGenerator.h"
#include "EventListener.h"
#include "EngineComponent.h"

namespace BlazeEngine
{
	class LogManager : public EventGenerator, public EventListener, public EngineComponent
	{
	public:
		//LogManager();
		//~LogManager();
		static LogManager& Instance();

		void Startup(CoreEngine * coreEngine);

		void Shutdown();

		void Update();

		int HandleEvent(EVENT_TYPE event, EventGenerator* generator);


	private:
		
		
	};

}