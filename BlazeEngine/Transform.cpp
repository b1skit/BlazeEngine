#include "Transform.h"
#include <algorithm>

#include "gtc/constants.hpp"

#define GLM_ENABLE_EXPERIMENTAL 
#include "gtx/common.hpp"


using std::find;


// DEBUG:
#include <iostream>
using std::cout;


namespace BlazeEngine
{
	// Static members:
	const vec3 Transform::WORLD_RIGHT	= vec3(1, 0, 0);
	const vec3 Transform::WORLD_UP		= vec3(0, 1, 0);
	const vec3 Transform::WORLD_FORWARD = vec3(0, 0, -1);


	// Constructor:
	Transform::Transform()
	{
		/*parent = nullptr;*/

		children.reserve(10);

		/*position = vec3(0, 0, 0);*/

		/*localRotation		= { 0,0,0,0 };
		worldRotation		= { 0,0,0,0 };*/

		/*model				= mat4( 1.0f );*/

		//isDirty				= false; // TO DO: Implement this functionality
	}

	//Transform::~Transform()
	//{
	//}

	mat4 Transform::Model()
	{
		this->model = this->translation * this->rotation * this->scale;
		mat4 combinedModel = model;

		if (this->parent != nullptr)
		{
			combinedModel = this->parent->Model() * combinedModel;
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
		this->translation = glm::translate(translation, amount);
		
		vec4 result = translation * vec4(0.0f, 0.0f, 0.0f, 1.0f);
		this->position = result.xyz();
	}

	void Transform::SetPosition(vec3 position)
	{
		this->translation = glm::translate(mat4(1.0f), position);
		this->position = position;
	}

	void Transform::Rotate(vec3 eulerXYZ)
	{
		if (eulerXYZ.x != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.x, WORLD_RIGHT);
		}
		if (eulerXYZ.y != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.y, WORLD_UP);
		}
		if (eulerXYZ.z != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.z, WORLD_FORWARD);
		}

		// Update our local CS axis:
		this->forward	= (rotation * vec4(WORLD_FORWARD, 0)).xyz();
		this->right		= (rotation * vec4(WORLD_RIGHT, 0)).xyz();
		this->up		= (rotation * vec4(WORLD_UP, 0)).xyz();

		this->eulerRotation += eulerXYZ;
		eulerRotation.x = glm::fmod<float>(eulerRotation.x, glm::two_pi<float>());
		eulerRotation.y = glm::fmod<float>(eulerRotation.y, glm::two_pi<float>());
		eulerRotation.z = glm::fmod<float>(eulerRotation.z, glm::two_pi<float>());
	}

	void Transform::SetEulerRotation(vec3 eulerXYZ)
	{
		this->rotation = mat4(1.0f);

		if (eulerXYZ.x != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.x, WORLD_RIGHT);
		}
		if (eulerXYZ.y != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.y, WORLD_UP);
		}
		if (eulerXYZ.z != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.z, WORLD_FORWARD);
		}

		// Update our local CS axis:
		this->forward = (rotation * vec4(WORLD_FORWARD, 0)).xyz();
		this->right = (rotation * vec4(WORLD_RIGHT, 0)).xyz();
		this->up = (rotation * vec4(WORLD_UP, 0)).xyz();

		this->eulerRotation = eulerXYZ;
		eulerRotation.x = glm::fmod<float>(eulerRotation.x, glm::two_pi<float>());
		eulerRotation.y = glm::fmod<float>(eulerRotation.y, glm::two_pi<float>());
		eulerRotation.z = glm::fmod<float>(eulerRotation.z, glm::two_pi<float>());
	}



	void Transform::RegisterChild(Transform* child)
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

	



}

