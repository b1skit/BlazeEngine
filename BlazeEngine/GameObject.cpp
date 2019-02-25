#include "GameObject.h"

namespace BlazeEngine
{
	GameObject::GameObject(string name, Renderable renderable) : SceneObject::SceneObject(name)
	{
		this->renderable = renderable;
	}

	//GameObject::~GameObject()
	//{
	//}
	   
	void BlazeEngine::GameObject::Startup(CoreEngine * coreEngine)
	{

	}

	void BlazeEngine::GameObject::Shutdown()
	{

	}

	void BlazeEngine::GameObject::Update()
	{

	}

	void BlazeEngine::GameObject::HandleEvent(EventInfo const * eventInfo)
	{

	}
}

