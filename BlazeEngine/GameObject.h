// Game object interface
// Game object interface.
// All in-scene/game objects should inherit from this interface

#pragma once

#include "BlazeObject.h"
#include "EventListener.h"
#include "SceneManager.h"

namespace BlazeEngine
{
	class GameObject : public BlazeObject, public EventListener
	{
	public:
		GameObject(SceneManager* sceneManager, string name) : BlazeObject::BlazeObject(name)
		{
			this->sceneManager = sceneManager;
		}

		/*~GameObject();*/




	protected:


	private:
		static SceneManager* sceneManager;

	};
}


