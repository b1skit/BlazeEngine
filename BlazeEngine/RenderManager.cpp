#include "RenderManager.h"
#include "CoreEngine.h"

#include "GL/glew.h"
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

namespace BlazeEngine
{
	/*RenderManager::~RenderManager()
	{
	}*/

	RenderManager& RenderManager::Instance()
	{
		static RenderManager* instance = new RenderManager();
		return *instance;
	}

	void RenderManager::Render(double alpha)
	{


		SDL_GL_SwapWindow(glWindow);


		// Deubg:
		/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Pretending to render at ~60fps..." });*/
		SDL_Delay((unsigned int)(1000.0 / 60.0));

		
	}

	void RenderManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		// Cache the relevant config data:
		this->xRes = coreEngine->GetConfig()->renderer.windowXRes;
		this->yRes = coreEngine->GetConfig()->renderer.windowYRes;
		this->windowTitle = coreEngine->GetConfig()->renderer.windowTitle;

		// Configure and open a window:
		/*SDL_Init(SDL_INIT_VIDEO);*/ // Currently doing a global init... 

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		glWindow = SDL_CreateWindow(
			coreEngine->GetConfig()->renderer.windowTitle.c_str(), 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			xRes, 
			yRes, 
			SDL_WINDOW_OPENGL
		);

		glContext = SDL_GL_CreateContext(glWindow);

		GLenum glStatus = glewInit();
		if (glStatus != GLEW_OK)
		{
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ERROR, this, "Render manager start failed: glStatus not ok!" });
		}
		else
		{
			// Set the initial color in both buffers:
			glClearColor(0.79f, 0.32f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			SDL_GL_SwapWindow(glWindow);

			glClearColor(0.79f, 0.32f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager started!" });
		}		
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


}


