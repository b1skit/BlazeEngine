#pragma once
#include "EventGenerator.h"
#include "EventListener.h"

class LogManager : EventGenerator, EventListener
{
public:
	LogManager();
	~LogManager();

	int HandleEvent(EVENT_TYPE event, EventGenerator* generator);


private:

};

