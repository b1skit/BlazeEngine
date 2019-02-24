// Game object transformation component

#pragma once

#include "glm.hpp"
#include "gtc/quaternion.hpp"

using glm::vec3;
using glm::quat;
using glm::mat4;


namespace BlazeEngine
{
	enum SPACE
	{
		SPACE_LOCAL,
		SPACE_WORLD,
		SPACE_VIEW,
		SPACE_CLIP,
	};

	class Transform
	{
	public:
		Transform();
		/*~Transform();*/

		// Getters:
		vec3 LocalPosition();
		vec3 WorldPosition();

		quat LocalRotation();
		quat WorldRotation();

		mat4 Model(); // Local->World
		mat4 View(); // World->View
		mat4 Projection(); // View->Clip

		//// Functionality:
		//void Translate(vec3 amount, SPACE space);
		//void Rotate(quat amount, SPACE space);
		//void Scale(vec3 amount, SPACE space);

	protected:


	private:
		vec3 localPosition;
		vec3 worldPosition;
		
		quat localRotation;
		quat worldRotation;

		mat4 model;
		mat4 view;
		mat4 projection;

		bool isDirty;
	};
}


