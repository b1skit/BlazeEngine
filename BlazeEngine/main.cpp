#include <iostream>
#include "CoreEngine.h"

//using BlazeEngine::CoreEngine;
using std::cout;


int main(int argc, char **argv)
{
	cout << "Welcome to the Blaze Engine!\n\n";

	BlazeEngine::CoreEngine coreEngine("./"); // TO DO: Implement config file (command line) path passing

	coreEngine.Startup();

	coreEngine.Run();

	coreEngine.Shutdown();

	cout << "\nGoodbye!\n";

	return 0;
}