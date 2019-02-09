#pragma once
#include "EventGenerator.h"

namespace BlazeEngine
{

	class EventListener
	{
	public:
		virtual int HandleEvent(EventInfo eventInfo) = 0;

	private:

	};

}