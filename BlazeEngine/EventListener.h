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
		virtual void HandleEvent(EventInfo const* eventInfo) = 0;

	private:

	};

}