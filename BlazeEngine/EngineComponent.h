// Interface for Blaze engine components
// Inherits from BlazeObject

#pragma once
#include "BlazeObject.h"


namespace BlazeEngine
{
	// Forward declarations:
	class CoreEngine;

	// EngineComponent Interface: Functionality common to components in main game loop
	class EngineComponent : public BlazeObject
	{
	public:
		EngineComponent() : BlazeObject() {};

		virtual void Startup(CoreEngine* coreEngine) = 0;
		
		virtual void Shutdown() = 0;

		virtual void Update() = 0;


	protected:
		CoreEngine* coreEngine;

	private:

	};
}