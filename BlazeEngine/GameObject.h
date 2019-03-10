// Game object interface
// Game object interface.
// All in-scene/game objects should inherit from this interface

#pragma once

#include "SceneObject.h"
#include "Renderable.h"

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	//// Predeclaration:
	//class CoreEngine;

	class GameObject : public SceneObject
	{
	public:
		GameObject() : SceneObject::SceneObject("Unnamed GameObject")
		{
			this->renderable.SetTransform(&this->transform);
		}

		GameObject(string name) : SceneObject::SceneObject(name) 
		{
			this->renderable.SetTransform(&this->transform);
		}

		GameObject(string name, Renderable renderable);

		// Copy constructor:
		GameObject(const GameObject& gameObject)
		{
			this->renderable = gameObject.renderable;
			this->transform = gameObject.transform;

			this->renderable.SetTransform(&this->transform);
		}

		/*~GameObject();*/

		// BlazeObject interface:
		void Update() override { }

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo) {}

		// Getters/Setters:
		inline Renderable const* GetRenderable() { return &renderable; }
		

	protected:
		Renderable renderable;


	private:
		

		
	};
}


