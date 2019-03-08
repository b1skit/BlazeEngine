#include "Transform.h"
#include <algorithm>

using std::find;

namespace BlazeEngine
{
	Transform::Transform()
	{
		parent = nullptr;
		children.reserve(10);

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

	mat4 Transform::Model() const
	{
		mat4 combinedModel = this->model;

		Transform const* currentParent = this->parent;
		while (currentParent != nullptr)
		{
			combinedModel = currentParent->Model() * combinedModel;
			currentParent = currentParent->GetParent();
		}

		return combinedModel;
	}

	void Transform::SetParent(Transform* parent)
	{
		this->parent = parent;
		this->parent->RegisterChild(this);
	}

	void Transform::UnParent()
	{
		this->parent->UnregisterChild(this);
		this->parent = nullptr;
	}

	void Transform::Translate(vec3 amount)
	{
		this->model = glm::translate(model, amount);
	}

	void Transform::RegisterChild(Transform const* child)
	{
		if (find(children.begin(), children.end(), child) !=  children.end())
		{
			children.push_back(child);
		}
	}

	void Transform::UnregisterChild(Transform const* child)
	{
		for (unsigned int i = 0; i < child->children.size(); i++)
		{
			if (children.at(i) == child)
			{
				children.erase(children.begin() + i);
				break;
			}
		}
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

