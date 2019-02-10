#pragma once
//#include "EventGenerator.h"
//#include "EventManager.h"

namespace BlazeEngine
{
	// Predeclaration:
	struct EventInfo;

	class EventListener
	{
	public:
		virtual int HandleEvent(EventInfo eventInfo) = 0;

	private:

	};

}