// Game object interface
// Game object interface.
// All in-scene/game objects should inherit from this interface

#pragma once

#include "SceneObject.h"
#include "Renderable.h"

// Predeclaration:
class CoreEngine;


namespace BlazeEngine
{
	class GameObject : public SceneObject
	{
	public:
		GameObject() : SceneObject::SceneObject("Unnamed GameObject") {}
		GameObject(string name) : SceneObject::SceneObject(name) {};
		GameObject(string name, Renderable renderable);

		// Copy constructor:
		GameObject(const GameObject& gameObject)
		{
			this->renderable = gameObject.renderable;
		}

		/*~GameObject();*/

		// BlazeObject interface:
		void Startup(CoreEngine * coreEngine);
		void Shutdown();
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline Renderable const* GetRenderable() { return &renderable; }
		

	protected:
		Renderable renderable;


	private:
		

		
	};
}


