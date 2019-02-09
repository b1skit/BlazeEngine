#include <iostream>
#include "CoreEngine.h"

using BlazeEngine::CoreEngine;
using std::cout;


int main()
{
	cout << "Welcome to the Blaze Engine!\n";

	CoreEngine coreEngine;

	coreEngine.Startup();

	coreEngine.Run();

	coreEngine.Shutdown();

	cout << "Goodbye!\n";

	return 0;
}