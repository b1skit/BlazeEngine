// The Blaze rendering engine

#pragma once
#include "EngineComponent.h"
#include "EventManager.h"
#include <string>
//#include "glm.hpp"
#include "SDL.h"

namespace BlazeEngine
{
	
	class RenderManager : public EngineComponent
	{
	public:
		RenderManager() : EngineComponent("RenderManager") {}
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
		// Configuration:
		int xRes;
		int yRes;
		string windowTitle;
		SDL_Window* glWindow;
		SDL_GLContext glContext;
		
		

	};
}


