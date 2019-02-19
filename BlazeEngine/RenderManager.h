// The Blaze rendering engine

#pragma once
#include "EngineComponent.h"
#include "EventManager.h"
#include "Shader.h"
#include <string>
#include "glm.hpp"
#include "SDL.h"

using glm::vec4;


namespace BlazeEngine
{
	
	class RenderManager : public EngineComponent
	{
	public:
		RenderManager() : EngineComponent("RenderManager") {}

		~RenderManager();

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
		
		// OpenGL components and settings:
		SDL_Window* glWindow;
		SDL_GLContext glContext;
		unsigned int VBO = 0; // Vertex Buffer Object: Holds vertices in GPU memory

		
		// DEBUG: Temporarily hard coded shader
		Shader* defaultShader;
		string filepath = "./Shaders/defaultShader";


		// Private member functions:
		void ClearWindow(vec4 clearColor);

	};
}


