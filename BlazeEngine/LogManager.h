#pragma once
//#include "EventGenerator.h"
#include "EventListener.h"
#include "EngineComponent.h"

namespace BlazeEngine
{
	class LogManager : public EngineComponent, public EventListener
	{
	public:
		LogManager();
		//~LogManager();
		static LogManager& Instance();

		// Disallow copying of our Singleton
		LogManager(LogManager const&) = delete;
		void operator=(LogManager const&) = delete;		

		// EngineComponent:
		void Startup(CoreEngine * coreEngine);

		void Shutdown();

		void Update();

		// EventListener:
		int HandleEvent(EventInfo eventInfo);

	private:
		
		
	};

}