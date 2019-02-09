#pragma once
#include <string>

using std::string;

namespace BlazeEngine
{
	enum EVENT_TYPE
	{
		EVENT_LOG = 0,
		EVENT_ERROR = 1,
		// EVENT_TICK ??
		// EVENT_UPDATE ??
		// ...

	}; // NUM_EVENT_TYPES must be the same as the number of EVENT_TYPE enums
	const static int NUM_EVENT_TYPES = 2; // Must equal the number of EVENT_TYPE enums
	
	

	class EventGenerator; // Pre-declaration
	struct EventInfo
	{
		EVENT_TYPE type;
		EventGenerator* generator;
		string eventMessage = ""; // Default to empty message
	};

	

	class EventGenerator
	{
	public:
		virtual int GetObjectID() = 0;


	private:

	};

}