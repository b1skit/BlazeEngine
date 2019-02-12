#include "LogManager.h"
#include <iostream>
#include "EventManager.h"
#include "CoreEngine.h"

using std::cout;

namespace BlazeEngine
{
	LogManager::LogManager() : EngineComponent()
	{
		SetName("LogManager");
	}


	//LogManager::~LogManager()
	//{
	//
	//}

	LogManager& LogManager::Instance()
	{
		static LogManager* instance = new LogManager();
		return *instance;
	}

	void LogManager::Startup(CoreEngine* coreEngine) 
	{
		EngineComponent::Startup(coreEngine);

		this->coreEngine->BlazeEventManager->Subscribe(EVENT_LOG, this);

		this->coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Log manager started!" });
	}

	void LogManager::Shutdown()
	{
		coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Log manager shutting down..." });
	}

	void LogManager::Update()
	{
		
	}


	int LogManager::HandleEvent(EventInfo eventInfo)
	{
		switch (eventInfo.type)
		{
		case EVENT_LOG:
			cout << "EVENT_LOG: Object #" << std::to_string(eventInfo.generator->GetObjectID() ) << " \"" << eventInfo.generator->GetName() <<"\": \"" << eventInfo.eventMessage << "\"\n";
			break;

		default:
			cout << "ERROR: Default event generated!\n";
			break;
		}

		return 0;
	}
}