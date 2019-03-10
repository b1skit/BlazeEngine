// Scene object interface

#pragma once

#include "BlazeObject.h"
#include "EventListener.h"
#include "Transform.h"


namespace BlazeEngine
{
	// Predeclarations:
	class CoreEngine;

	class SceneObject : public BlazeObject, public EventListener
	{
	public:
		SceneObject() : BlazeObject::BlazeObject("Unnamed SceneObject") {}
		SceneObject(string newName) : BlazeObject::BlazeObject(newName) {}

		// Copy constructor:
		SceneObject(const SceneObject& sceneObject) : BlazeObject(sceneObject.GetName())
		{
			this->transform = sceneObject.transform;
		}

		inline Transform* GetTransform() { return &transform; }


	protected:
		Transform transform;
		

	private:
		


	};
}
