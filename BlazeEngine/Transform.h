// Game object transformation component

#pragma once

#include <vector>
#include "glm.hpp"
#include "gtc/quaternion.hpp"

using glm::vec3;
using glm::quat;
using glm::mat4;
using std::vector;


namespace BlazeEngine
{
	//enum SPACE
	//{
	//	SPACE_LOCAL,
	//	SPACE_WORLD,
	//	SPACE_VIEW,
	//	SPACE_CLIP,
	//};

	class Transform
	{
	public:
		Transform();
		/*~Transform();*/

		// TO DO: Copy constructor ??

		// Getters/Setters:
		/*inline Transform* Parent() { return parent; }*/
		/*inline vec3& LocalPosition() { return localPosition; }
		inline vec3& WorldPosition() { return worldPosition; }*/

		/*quat LocalRotation();
		quat WorldRotation();*/

		inline mat4 const* Model() { return &model; } // Local->World

		//inline mat4& View() { return view; } // World->View
		//inline mat4& Projection() { return projection; } // View->Clip

		/*inline vec3& LocalPosition() { return localPosition; }*/

		//// Functionality:
		/*void SetPosition(vec3 position);*/
		//void Translate(vec3 amount, SPACE space);

		/*void SetRotation(quat rotation);*/
		//void Rotate(quat amount, SPACE space);

		/*void SetScale(vec3 scale);*/
		//void Scale(vec3 amount, SPACE space);
		

	protected:


	private:
		/*Transform* parent = nullptr;
		vector<Transform*> children;*/

		//vec3 localPosition;
		//vec3 worldPosition;
		
		/*quat localRotation;
		quat worldRotation;*/

		mat4 model = mat4( 1.0f );
		/*mat4 view;
		mat4 projection;
		mat4 modelView;
		mat4 modelViewProjection;*/

		/*bool isDirty;*/
	};
}


