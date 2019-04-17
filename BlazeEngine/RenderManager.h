// The Blaze rendering engine

#pragma once

#include "EngineComponent.h"
#include "EventManager.h"
#include "Shader.h"

#include <string>


#include "SDL.h"

#define GLM_FORCE_SWIZZLE
#include "glm.hpp"

using glm::vec4;


namespace BlazeEngine
{
	enum SHADER // Guaranteed shaders
	{
		SHADER_ERROR = 0,
		SHADER_DEFAULT = 1,
	};

	
	class RenderManager : public EngineComponent
	{
	public:
		RenderManager() : EngineComponent("RenderManager") {}

		~RenderManager();

		// Singleton functionality:
		static RenderManager& Instance();
		RenderManager(RenderManager const&) = delete; // Disallow copying of our Singleton
		void operator=(RenderManager const&) = delete;

		// EngineComponent interface:
		void Startup();

		void Shutdown();

		void Update();

		// Member functions:
		void Render(double alpha);

		
	private:
		// Configuration:
		int xRes;
		int yRes;
		string windowTitle;
		
		// OpenGL components and settings:
		SDL_Window* glWindow;
		SDL_GLContext glContext;
		
		// Private member functions:
		void ClearWindow(vec4 clearColor);
	};
}


