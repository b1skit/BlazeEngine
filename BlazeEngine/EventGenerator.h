#pragma once
#include "BlazeObject.h"
#include <string>

using std::string;

namespace BlazeEngine
{
	class EventGenerator; // Pre-declaration


	enum EVENT_TYPE
	{
		EVENT_LOG = 0,
		EVENT_ERROR = 1,
		// EVENT_TICK ??
		// EVENT_UPDATE ??
		// ...
	};
	const static int NUM_EVENT_TYPES = 2; // MUST equal the number of EVENT_TYPE enums

	struct EventInfo
	{
		EVENT_TYPE type;
		BlazeObject* generator;
		string eventMessage = ""; // Default to empty message
	};

	

	class EventGenerator
	{
	public:
		


	private:

	};

}