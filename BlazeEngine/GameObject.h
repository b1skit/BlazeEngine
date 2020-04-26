// Game object interface
// Game object interface.
// All in-scene/game objects should inherit from this interface

#pragma once

#include "SceneObject.h"	// Base class
#include "Renderable.h"


namespace BlazeEngine
{
	class GameObject : public SceneObject
	{
	public:
		// No-arg constructor (Don't use this!):
		GameObject() : SceneObject::SceneObject("Unnamed GameObject")
		{
			this->renderable.SetTransform(&this->transform);
		}

		// String constructor:
		GameObject(string name) : SceneObject::SceneObject(name) 
		{
			this->renderable.SetTransform(&this->transform);
		}

		// String and renderable constructor:
		GameObject(string name, Renderable renderable);

		// Copy constructor:
		GameObject(const GameObject& gameObject)
		{
			this->renderable = gameObject.renderable;
			this->transform = gameObject.transform;

			this->renderable.SetTransform(&this->transform);
		}

		// BlazeObject interface:
		void Update() override { }

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo) {}

		// Getters/Setters:
		inline Renderable* GetRenderable() { return &renderable; }
		

	protected:
		Renderable renderable;


	private:
		

		
	};
}


