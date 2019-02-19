#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"

#include "glm.hpp"
#include "GL/glew.h"
#include <GL/GL.h> // MUST follow glew.h...
#include <SDL_egl.h>
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

using glm::vec3;
using glm::vec4;


namespace BlazeEngine
{
	RenderManager::~RenderManager()
	{
		delete defaultShader; // DEBUG: Delete our hard-coded shader
	}

	RenderManager& RenderManager::Instance()
	{
		static RenderManager* instance = new RenderManager();
		return *instance;
	}

	void RenderManager::Render(double alpha)
	{
		vec3 vertices[3] = 
		{
			{-0.5f, -0.5f, 0.0f},
			{0.5f, -0.5f, 0.0f},
			{0.0f, 0.5f, 0.0f}
		};

		// Copy vertices to the currently bound buffer:
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		


		// Display the new frame:
		SDL_GL_SwapWindow(glWindow);


		// Debug:
		SDL_Delay((unsigned int)(1000.0 / 60.0));

		
	}

	void RenderManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		// Cache the relevant config data:
		this->xRes = coreEngine->GetConfig()->renderer.windowXRes;
		this->yRes = coreEngine->GetConfig()->renderer.windowYRes;
		this->windowTitle = coreEngine->GetConfig()->renderer.windowTitle;

		// Initialize SDL:
		/*SDL_Init(SDL_INIT_VIDEO);*/ // Currently doing a global init... 

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Create a window:
		glWindow = SDL_CreateWindow(
			coreEngine->GetConfig()->renderer.windowTitle.c_str(), 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			xRes, 
			yRes, 
			SDL_WINDOW_OPENGL
		);

		// Create an OpenGL context:
		glContext = SDL_GL_CreateContext(glWindow);
		
		// Initialize glew:
		GLenum glStatus = glewInit();
		if (glStatus != GLEW_OK)
		{
			/*this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "Render manager start failed: glStatus not ok!" });*/
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, "Render manager start failed: glStatus not ok!" });
			return;
		}

		// Create and bind a vertex buffer:
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));

		
		// DEBUG: Create a shader
		defaultShader = new Shader(filepath);


		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager started!" });
		
	}

	void RenderManager::Shutdown()
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager shutting down..." });

		// Close our window:
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(glWindow);
		SDL_Quit();
	}

	void RenderManager::Update()
	{
		
	}

	void RenderManager::ClearWindow(vec4 clearColor)
	{
		// Set the initial color in both buffers:
		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(glWindow);

		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT);
	}


}


