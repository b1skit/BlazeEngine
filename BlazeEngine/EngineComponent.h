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
		EngineComponent(string name) : BlazeObject(name)
		{
			coreEngine = nullptr; // Init to null: Overridden when startup is called
		}

		// We can't control the order constructors are called, so this function should be called to start the object
		virtual void Startup() = 0;
		
		virtual void Shutdown() = 0;

		/*virtual void Update() = 0;*/


	protected:


	private:


	};
}