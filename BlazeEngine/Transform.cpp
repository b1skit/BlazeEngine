#include "Transform.h"

namespace BlazeEngine
{
	Transform::Transform()
	{
		/*parent = nullptr;
		children.reserve(10);*/

		/*localPosition		= { 0, 0, 0 };
		worldPosition		= { 0, 0, 0 };*/

		/*localRotation		= { 0,0,0,0 };
		worldRotation		= { 0,0,0,0 };*/

		model				= mat4( 1.0f );
		/*view				= mat4( 1.0f );
		projection			= mat4( 1.0f );
		modelView			= mat4( 1.0f );
		modelViewProjection = mat4( 1.0f );
*/
		//isDirty				= false; // TO DO: Implement this functionality
	}


	//Transform::~Transform()
	//{
	//}

	//vec3 Transform::LocalPosition()
	//{
	//	return localPosition;
	//}

	//vec3 Transform::WorldPosition()
	//{
	//	return worldPosition;
	//}

	//quat Transform::LocalRotation()
	//{
	//	return localRotation;
	//}

	//quat Transform::WorldRotation()
	//{
	//	return worldRotation;
	//}

	//mat4 Transform::Model()
	//{
	//	return model;
	//}

	//mat4 Transform::View()
	//{
	//	return view;
	//}

	//mat4 Transform::Projection()
	//{
	//	return projection;
	//}

	//void Transform::Translate(vec3 amount, SPACE space)
	//{

	//	isDirty = true;
	//}

	//void Transform::Rotate(quat amount, SPACE space)
	//{

	//	isDirty = true;
	//}

	//void Transform::Scale(vec3 amount, SPACE space)
	//{

	//	isDirty = true;
	//}

}

