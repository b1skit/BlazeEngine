#pragma once
#include "EventListener.h"
#include "EngineComponent.h"

namespace BlazeEngine
{
	class LogManager : public EngineComponent, public EventListener
	{
	public:
		LogManager() : EngineComponent("LogManager") {}
		//~LogManager();
		
		// Singleton functionality:
		static LogManager& Instance();
		LogManager(LogManager const&) = delete; // Disallow copying of our Singleton
		void operator=(LogManager const&) = delete;		

		// EngineComponent interface:
		void Startup(CoreEngine * coreEngine);

		void Shutdown();

		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

	private:
		
		
	};

}