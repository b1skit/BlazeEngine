#pragma once
#include "EventListener.h"
#include "EngineComponent.h"


// Compiler directives: Control logging verbosity
//#define LOG_VERBOSITY_ALL
#define LOG_VERBOSITY_DEBUG
//#define LOG_VERBOSITY_RELEASE

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
		void Startup();
		void Shutdown();
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

	private:
		
		
	};

}