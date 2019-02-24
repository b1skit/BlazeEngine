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
		GameObject(string newName) : SceneObject::SceneObject(newName) {};

		/*~GameObject();*/

		// BlazeObject interface:
		void Startup(CoreEngine * coreEngine);
		void Shutdown();
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline Renderable const* GetRenderable() { return renderable; }
		inline void SetRenderable(Renderable* renderable) { this->renderable = renderable; }

	protected:
		Renderable* renderable;


	private:
		

		
	};
}


