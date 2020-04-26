// Scene object interface

#pragma once

#include "BlazeObject.h"	// Base class
#include "EventListener.h"	// Base class
#include "Transform.h"


namespace BlazeEngine
{
	class SceneObject : public BlazeObject, public EventListener
	{
	public:
		SceneObject() : BlazeObject::BlazeObject("Unnamed SceneObject") { }
		SceneObject(string newName) : BlazeObject::BlazeObject(newName) {}

		// Copy constructor:
		SceneObject(const SceneObject& sceneObject) : BlazeObject(sceneObject.GetName())
		{
			this->transform = sceneObject.transform;
		}

		// BlazeObject interface:
		/*void Update() { BlazeObject::Update(); }*/

		// Getters/Setters:
		inline Transform* GetTransform() { return &transform; }


	protected:
		Transform transform;
		

	private:
		


	};
}
