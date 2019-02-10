#include <iostream>
#include "CoreEngine.h"

using BlazeEngine::CoreEngine;
using std::cout;


int main()
{
	cout << "Welcome to the Blaze Engine!\n\n";

	CoreEngine coreEngine;

	coreEngine.Startup();

	coreEngine.Run();

	coreEngine.Shutdown();

	cout << "\nGoodbye!\n";

	return 0;
}