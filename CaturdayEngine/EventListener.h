#pragma once
#include "EventManager.h"
#include "EventGenerator.h"

class EventListener
{
public:
	virtual int HandleEvent(EVENT_TYPE event, EventGenerator* generator) = 0;
	
private:

};