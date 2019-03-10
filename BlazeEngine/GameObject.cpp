#include "GameObject.h"

namespace BlazeEngine
{
	GameObject::GameObject(string name, Renderable renderable) : SceneObject::SceneObject(name)
	{
		this->renderable = renderable;
		this->renderable.SetTransform(&this->transform);
	}

	//GameObject::~GameObject()
	//{
	//}

	//void BlazeEngine::GameObject::Update()
	//{

	//}

	//void BlazeEngine::GameObject::HandleEvent(EventInfo const * eventInfo)
	//{

	//}
}

