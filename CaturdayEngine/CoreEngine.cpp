
#include <iostream>
#include "EventManager.h"
#include "LogManager.h"

using std::cout;
using std::endl;


namespace CaturdayEngine
{
	const static EventManager _EventManager;
	const static LogManager _LogManager;


	int main()
	{
		cout << "Welcome to the Caturday Engine..." << endl;

		return 0;
	}

	void Startup()
	{
		cout << "CoreEngine starting up...\n";
	}

	void Shutdown()
	{
		cout << "CoreEngine shutting down...\n";
	}


}