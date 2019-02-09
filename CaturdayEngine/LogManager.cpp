#include "LogManager.h"
#include <iostream>
#include "EventManager.h"

using std::cout;
using std::endl;


namespace BlazeEngine
{
	extern EventManager _EventManager;


	//LogManager::LogManager()
	//{
	//	_EventManager.Instance();
	//}


	//LogManager::~LogManager()
	//{

	//}


	int LogManager::HandleEvent(EVENT_TYPE event, EventGenerator * generator)
	{
		switch (event)
		{
		case EVENT_TICK:
			cout << "EVENT_UPDATE posted by generator  ???" << endl;
			break;

		default:
			cout << "ERROR: Default event generated!" << endl;
			break;
		}

		return 0;
	}

}