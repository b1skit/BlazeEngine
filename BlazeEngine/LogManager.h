#pragma once
#include "EventListener.h"		// Base class
#include "EngineComponent.h"	// Base class


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

		// Static functions:
		//------------------

		static void Log(string const& message);
		static void LogWarning(string const& message);
		static void LogError(string const& message);

	private:
		
		
	};
}