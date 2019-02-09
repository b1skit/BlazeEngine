#pragma once
//#include "CoreEngine.h"


namespace BlazeEngine
{
	// Forward declarations:
	class CoreEngine;

	// EngineComponent Interface: Functionality common to components in main game loop
	class EngineComponent
	{
	public:
		virtual void Startup(CoreEngine* coreEngine) = 0;
		virtual void Shutdown() = 0;

		virtual void Update() = 0;


	protected:
		//static CoreEngine* coreEngine;
		const CoreEngine* coreEngine;


	private:

	};
}