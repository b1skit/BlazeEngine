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
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_ERROR, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_ENGINE_QUIT, this);

		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP, this);
		
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_CLICK, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_RELEASE, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_MOVED, this);


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
		cout << EVENT_NAME[eventInfo.type] << ": Object #" << std::to_string(eventInfo.generator->GetObjectID()) << " (" << eventInfo.generator->GetName() << ")";
		
		if (eventInfo.eventMessage.size() > 0)
		{
			cout << ": \"" << eventInfo.eventMessage << "\"";
		}

		cout << std::endl;
		
		return 0;
	}
}