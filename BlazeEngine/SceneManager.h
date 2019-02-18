#pragma once

#include "EngineComponent.h"
#include "EventListener.h"

namespace BlazeEngine
{
	class SceneManager : public EngineComponent, public EventListener
	{
	public:
		SceneManager() : EngineComponent("SceneManager") {}
		/*~SceneManager();*/

		// Singleton functionality:
		static SceneManager& Instance();
		SceneManager(SceneManager const&) = delete; // Disallow copying of our Singleton
		void operator=(SceneManager const&) = delete;

		// EngineComponent interface:
		void Startup(CoreEngine * coreEngine);

		void Shutdown();

		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Member functions:
		void UpdateSceneObjects();

	protected:


	private:


	};

}

