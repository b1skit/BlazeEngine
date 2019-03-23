#include "Transform.h"
#include <algorithm>

#include "gtc/constants.hpp"

#define GLM_ENABLE_EXPERIMENTAL 
#include "gtx/common.hpp"

using glm::normalize;

using std::find;


// DEBUG:
#include <iostream>
using std::cout;


namespace BlazeEngine
{
	// Static members:
	const vec3 Transform::WORLD_X	= vec3(1, 0, 0);
	const vec3 Transform::WORLD_Y	= vec3(0, 1, 0);
	const vec3 Transform::WORLD_Z	= vec3(0, 0, 1); // Note: BlazeEngine always uses a RHCS


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
		this->model = this->translation * this->scale * this->rotation;
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
		this->translation = glm::translate(this->translation, amount);
		
		vec4 result = translation * vec4(0.0f, 0.0f, 0.0f, 1.0f);
		this->position = result.xyz();
	}

	void Transform::SetPosition(vec3 position)
	{
		this->translation = glm::translate(mat4(1.0f), position);
		this->position = position;
	}

	void Transform::Rotate(vec3 eulerXYZ) // Note: eulerXYZ is in RADIANS
	{
		if (eulerXYZ.x != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.x, WORLD_X);
		}
		if (eulerXYZ.y != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.y, WORLD_Y);
		}
		if (eulerXYZ.z != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.z, WORLD_Z);
		}

		// Update our local CS axis:
		this->right		= normalize((this->rotation * vec4(WORLD_X, 0)).xyz());
		this->up		= normalize((this->rotation * vec4(WORLD_Y, 0)).xyz());
		this->forward	= normalize((this->rotation * vec4(WORLD_Z, 0)).xyz());

		this->eulerRotation += eulerXYZ;
		
		// Keep our xyz in [0, 2pi]:
		eulerRotation.x = glm::fmod<float>(eulerRotation.x, glm::two_pi<float>()); 
		eulerRotation.y = glm::fmod<float>(eulerRotation.y, glm::two_pi<float>());
		eulerRotation.z = glm::fmod<float>(eulerRotation.z, glm::two_pi<float>());
	}

	void Transform::SetEulerRotation(vec3 eulerXYZ)
	{
		this->rotation = mat4(1.0f);

		if (eulerXYZ.x != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.x, WORLD_X);
		}
		if (eulerXYZ.y != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.y, WORLD_Y);
		}
		if (eulerXYZ.z != 0)
		{
			this->rotation = glm::rotate(this->rotation, eulerXYZ.z, WORLD_Z);
		}

		// Update our local CS axis:
		this->right		= normalize((rotation * vec4(WORLD_X, 0)).xyz());
		this->up		= normalize((rotation * vec4(WORLD_Y, 0)).xyz());
		this->forward	= normalize((rotation * vec4(WORLD_Z, 0)).xyz());

		this->eulerRotation = eulerXYZ;

		// Keep our xyz in [0, 2pi]:
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

