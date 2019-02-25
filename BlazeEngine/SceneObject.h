// Scene object interface

#pragma once

#include "BlazeObject.h"
#include "EventListener.h"
#include "Transform.h"


namespace BlazeEngine
{
	class SceneObject : public BlazeObject, public EventListener
	{
	public:
		SceneObject() : BlazeObject::BlazeObject("Unnamed SceneObject") {}
		SceneObject(string newName) : BlazeObject::BlazeObject(newName) {}

		inline Transform* GetTransform()
		{
			return &transform;
		}

	protected:
		Transform transform;

	private:
		


	};
}
