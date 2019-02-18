#include "LogManager.h"
#include <iostream>
#include "EventManager.h"
#include "CoreEngine.h"

using std::cout;

namespace BlazeEngine
{
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

		// Subscribe to every event type:
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_LOG, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_ERROR, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_ENGINE_QUIT, this);

		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_FORWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_FORWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_BACKWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_BACKWARD, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_RIGHT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_RIGHT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_UP, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_UP, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_DOWN_DOWN, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_BUTTON_UP_DOWN, this);
		
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_CLICK_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_RELEASE_LEFT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_CLICK_RIGHT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_RELEASE_RIGHT, this);
		this->coreEngine->BlazeEventManager->Subscribe(EVENT_INPUT_MOUSE_MOVED, this);


		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Log manager started!" });
	}

	void LogManager::Shutdown()
	{
		coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Log manager shutting down..." });
	}

	void LogManager::Update()
	{
		
	}


	void LogManager::HandleEvent(EventInfo const* eventInfo)
	{
		cout << EVENT_NAME[eventInfo->type] << ": Object #";
		
		if (eventInfo->generator)
		{
			cout << std::to_string(eventInfo->generator->GetObjectID()) << " (" << eventInfo->generator->GetName() << ")";
		}
		else
		{
			cout << "_unknown_ (null event generator)";
		}

		if (eventInfo->eventMessage.length() > 0)
		{
			cout << ": \"" << eventInfo->eventMessage << "\"";
		}

		cout << std::endl;
		
		return;
	}
}