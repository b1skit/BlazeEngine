#pragma once
#include "EventGenerator.h"
#include "EventListener.h"
#include "EngineComponent.h"

namespace BlazeEngine
{
	class LogManager : public EngineComponent, public EventGenerator, public EventListener
	{
	public:
		LogManager();
		//~LogManager();
		static LogManager& Instance();

		// EngineComponent:
		void Startup(CoreEngine * coreEngine, int objectID);

		void Shutdown();

		void Update();

		
		// EventGenerator:
		int GetObjectID();


		// EventListener:
		int HandleEvent(EventInfo eventInfo);

	private:
		
		
	};

}