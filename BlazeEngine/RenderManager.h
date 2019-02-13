// The Blaze rendering engine

#pragma once
#include "EngineComponent.h"
#include "EventManager.h"
#include "SDL.h"

namespace BlazeEngine
{
	class RenderManager : public EngineComponent
	{
	public:
		RenderManager();
		/*~RenderManager();*/

		// Singleton functionality:
		static RenderManager& Instance();
		RenderManager(RenderManager const&) = delete; // Disallow copying of our Singleton
		void operator=(RenderManager const&) = delete;

		// Member functions:
		void Render(double alpha);

		// EngineComponent interface:
		void Startup(CoreEngine * coreEngine);

		void Shutdown();

		void Update();

		
	private:
		SDL_Window* window;

		// Configuration:


	};
}


