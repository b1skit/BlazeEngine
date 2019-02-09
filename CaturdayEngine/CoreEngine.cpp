
#include <iostream>
#include "EventManager.h"
#include "LogManager.h"

using std::cout;
using std::endl;


int main()
{
	cout << "Welcome to the Blaze Engine..." << endl;

	return 0;
}

namespace BlazeEngine
{
	

	class CoreEngine
	{
	public:
		// Engine components:
		const static EventManager _EventManager;
		const static LogManager _LogManager;

		void Startup()
		{
			cout << "CoreEngine starting up...\n";

			return;
		}

		void Shutdown()
		{
			cout << "CoreEngine shutting down...\n";

			return;
		}

	private:


	};


	


}