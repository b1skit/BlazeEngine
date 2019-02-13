#include "RenderManager.h"
#include "CoreEngine.h"
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

namespace BlazeEngine
{
	RenderManager::RenderManager() : EngineComponent()
	{
		SetName("RenderManager");
	}

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
		/*coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Pretending to render at ~60fps..." });*/
		SDL_Delay((unsigned int)(1000.0 / 60.0));


	}

	void RenderManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		window = SDL_CreateWindow("Window title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 800, SDL_WINDOW_OPENGL);

		this->coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Render manager started!" });
	}

	void RenderManager::Shutdown()
	{
		this->coreEngine->BlazeEventManager->Notify(EventInfo{ EVENT_LOG, this, "Render manager shutting down..." });
	}

	void RenderManager::Update()
	{

	}


}


