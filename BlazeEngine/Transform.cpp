#include "BuildConfiguration.h"
#include "Transform.h"
#include <algorithm>

#if defined(DEBUG_LOG_OUTPUT)
	#include <iostream>
	using std::to_string;
#endif

#include "gtc/constants.hpp"

#define GLM_ENABLE_EXPERIMENTAL 
#include "gtx/common.hpp"

using glm::normalize;
using glm::rotate;

using std::find;


namespace BlazeEngine
{
	// Static members:
	//----------------
	const vec3 Transform::WORLD_X	= vec3(1, 0, 0);
	const vec3 Transform::WORLD_Y	= vec3(0, 1, 0);
	const vec3 Transform::WORLD_Z	= vec3(0, 0, 1); // Note: BlazeEngine always uses a RHCS


	// Constructor:
	Transform::Transform()
	{
		children.reserve(10);

		isDirty				= true;
	}

	//Transform::~Transform()
	//{
	//}


	mat4 Transform::Model()
	{
		if (isDirty)
		{
			this->model = this->translation * this->scale * this->rotation;

			this->combinedModel = model;
			if (this->parent != nullptr)
			{
				combinedModel = this->parent->Model() * combinedModel;
			}

			isDirty = false;
		}	

		return combinedModel;
	}

	mat4 Transform::ModelRotation()
	{
		return rotation;
	}


	void Transform::SetParent(Transform* parent)
	{
		if (parent != nullptr)
		{
			this->parent = parent;
			this->parent->RegisterChild(this);
			
			MarkDirty();
		}	
	}


	void Transform::UnParent()
	{
		this->parent->UnregisterChild(this);
		this->parent = nullptr;

		MarkDirty();
	}


	void Transform::Translate(vec3 amount)
	{
		this->translation	= glm::translate(this->translation, amount);
		
		vec4 result			= translation * vec4(0.0f, 0.0f, 0.0f, 1.0f); // TODO: Just extract the translation from the end column of the matrix???
		this->worldPosition	= result.xyz();

		MarkDirty();
	}


	void Transform::SetPosition(vec3 position)
	{
		this->translation = glm::translate(mat4(1.0f), position);
		this->worldPosition = position;

		MarkDirty();
	}

	void Transform::LookAt(vec3 camForward, vec3 camUp)
	{
		LOG_ERROR("DEBUG: Transform.LookAt() is not correctly implemented yet!!!");

		camUp			= normalize(camUp);
		camForward		= normalize(camForward);
		vec3 camRight	= glm::cross(camForward, camUp);

		LOG_ERROR("DEBUG: LOOKAT CROSS RESULT = " + to_string(camRight.x) + " " + to_string(camRight.y) + " " + to_string(camRight.z));
		
		// Copy values into the rotation matrix:
		for (int i = 0; i < 3; i++)
		{
			this->rotation[0][i] = camRight[i];
			this->rotation[1][i] = camUp[i];
			this->rotation[2][i] = camForward[i];
		}

		this->right		= camRight;
		this->up		= camUp;
		this->forward	= camForward;

		// TODO: UPDATE EULER ROTATION!!!!
		LOG_ERROR("DEBUG: EULER ROTATION IS NOT SET!!! GetEulerRotation() RESULTS FOR THIS TRANSFORM ARE NOW WRONG!!!!!!!!!");

		MarkDirty();
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
		// TODO: Work out why I do this different in SetEulerRotation()....


		// Update the world-space orientation of our local CS axis:
		this->right		= normalize((this->rotation * vec4(WORLD_X, 0)).xyz());
		this->up		= normalize((this->rotation * vec4(WORLD_Y, 0)).xyz());
		this->forward	= normalize((this->rotation * vec4(WORLD_Z, 0)).xyz());
		
		// Update the rotation value, and keep xyz bound in [0, 2pi]:
		this->eulerWorldRotation += eulerXYZ;
		eulerWorldRotation.x = glm::fmod<float>(eulerWorldRotation.x, glm::two_pi<float>()); 
		eulerWorldRotation.y = glm::fmod<float>(eulerWorldRotation.y, glm::two_pi<float>());
		eulerWorldRotation.z = glm::fmod<float>(eulerWorldRotation.z, glm::two_pi<float>());

		// TODO: CONFIRM THIS WORKS FOR NEGATIVE NUMBERS!!! 
		// Is this even necessary here, or can I just assume I'll never pass "invalid" values?

		
		MarkDirty();
	}


	void Transform::SetEulerRotation(vec3 eulerXYZ)
	{
		this->rotation = mat4(1.0f);

		if (eulerXYZ.x != 0)
		{
			this->rotation = glm::rotate(mat4(1.0f), eulerXYZ.x, WORLD_X);
		}
		if (eulerXYZ.y != 0)
		{
			this->rotation = glm::rotate(mat4(1.0f), eulerXYZ.y, WORLD_Y) * rotation;
		}
		if (eulerXYZ.z != 0)
		{
			this->rotation = glm::rotate(mat4(1.0f), eulerXYZ.z, WORLD_Z) * rotation;
		}

		// Update our local CS axis:
		this->right		= normalize((rotation * vec4(WORLD_X, 0)).xyz());
		this->up		= normalize((rotation * vec4(WORLD_Y, 0)).xyz());
		this->forward	= normalize((rotation * vec4(WORLD_Z, 0)).xyz());

		this->eulerWorldRotation = eulerXYZ;

		// Keep our xyz in [0, 2pi]:
		eulerWorldRotation.x = glm::fmod<float>(eulerWorldRotation.x, glm::two_pi<float>());
		eulerWorldRotation.y = glm::fmod<float>(eulerWorldRotation.y, glm::two_pi<float>());
		eulerWorldRotation.z = glm::fmod<float>(eulerWorldRotation.z, glm::two_pi<float>());

		// TODO: CONFIRM THIS WORKS FOR NEGATIVE NUMBERS!!! 
		// Is this even necessary here, or can I just assume I'll never pass "invalid" values?


		MarkDirty();
	}


	void Transform::SetScale(vec3 scale)
	{
		this->worldScale = scale;
		this->scale = glm::scale(mat4(1.0f), scale);

		MarkDirty();
	}

	void Transform::MarkDirty()
	{
		this->isDirty = true;

		for (int i = 0; i < (int)children.size(); i++)
		{
			children.at(i)->MarkDirty();
		}
	}


	void Transform::DebugPrint()
	{
		#if defined(DEBUG_TRANSFORMS)
			LOG("[TRANSFORM DEBUG]\n\tPostition = " + to_string(worldPosition.x) + ", " + to_string(worldPosition.y) + ", " + to_string(worldPosition.z));
			LOG("Euler rotation = " + to_string(eulerWorldRotation.x) + ", " + to_string(eulerWorldRotation.y) + ", " + to_string(eulerWorldRotation.z) + " (radians)");
			LOG("Scale = " + to_string(worldScale.x) + ", " + to_string(worldScale.y) + ", " + to_string(worldScale.z));
		#else
				return;
		#endif
	}


	// Static helper functions:
	//-------------------------

	vec3& Transform::RotateVector(vec3& targetVector, float const & radians, vec3 const & axis)
	{
		mat4 rotation = glm::rotate(mat4(1.0f), radians, axis);
		
		targetVector = (rotation * vec4(targetVector, 0.0f)).xyz();
		return targetVector;
	}


	// Protected functions:
	//---------------------

	void Transform::RegisterChild(Transform* child)
	{
		if (find(children.begin(), children.end(), child) ==  children.end())
		{
			children.push_back(child);

			MarkDirty();
		}
	}


	void Transform::UnregisterChild(Transform const* child)
	{
		for (unsigned int i = 0; i < child->children.size(); i++)
		{
			if (children.at(i) == child)
			{
				children.erase(children.begin() + i);
				MarkDirty();
				break;
			}
		}
	}
}

