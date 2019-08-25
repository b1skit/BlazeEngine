#include "BuildConfiguration.h"
#include "Transform.h"
#include <algorithm>

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
	const vec3 Transform::WORLD_X	= vec3(1.0f,	0.0f,	0.0f);
	const vec3 Transform::WORLD_Y	= vec3(0.0f,	1.0f,	0.0f);
	const vec3 Transform::WORLD_Z	= vec3(0.0f,	0.0f,	1.0f); // Note: BlazeEngine always uses a RHCS


	// Constructor:
	Transform::Transform()
	{
		children.reserve(10);

		isDirty	= true;
	}


	mat4 Transform::Model(MODEL_MATRIX_COMPONENT component /*= WORLD_MODEL*/)
	{
		if (isDirty)
		{
			Recompute();
		}	

		// Return the *combined* world transformations of the entire hierarchy
		switch (component)
		{
		case WORLD_TRANSLATION:
			return this->combinedTranslation;
			break;

		case WORLD_SCALE:
			return this->combinedScale;
			break;

		case WORLD_ROTATION:
			return this->combinedRotation;
			break;

		case WORLD_MODEL:
		default:
			return this->combinedModel;
		}		
	}


	void Transform::Parent(Transform* newParent)
	{
		// Unparent:
		if (newParent == nullptr)
		{
			if (this->parent != nullptr)
			{
				this->parent->UnregisterChild(this);
				this->parent = nullptr;
			}
		}
		// Parent:
		else 
		{
			this->parent = newParent;
			this->parent->RegisterChild(this);
		}
		
		MarkDirty();
	}


	void Transform::Translate(vec3 amount)
	{
		this->translation	= glm::translate(this->translation, amount);
		
		vec4 result			= translation * vec4(0.0f, 0.0f, 0.0f, 1.0f); // TODO: Just extract the translation from the end column of the matrix???
		this->worldPosition	= result.xyz();

		MarkDirty();
	}


	void Transform::SetWorldPosition(vec3 position)
	{
		this->translation	= glm::translate(mat4(1.0f), position);
		this->worldPosition = position;

		MarkDirty();
	}


	vec3 const& Transform::WorldPosition()
	{
		if (isDirty)
		{
			Recompute();
		}

		return worldPosition;
	}


	void Transform::Rotate(vec3 eulerXYZ) // Note: eulerXYZ is in RADIANS
	{
		// Concatenate rotations as quaternions:
		this->worldRotation = this->worldRotation * glm::quat(eulerXYZ);

		this->rotation = glm::mat4_cast(this->worldRotation);

		// Update the world-space orientation of our local CS axis:
		UpdateLocalAxis();

		// Update the rotation value, and keep xyz bound in [0, 2pi]:
		this->eulerWorldRotation += eulerXYZ;
		BoundEulerAngles();

		MarkDirty();
	}


	vec3 const&	Transform::GetEulerRotation()
	{ 
		if (isDirty)
		{
			Recompute(); // TODO: Must actually compute the eulerRotation in here!!!
		}

		return eulerWorldRotation; // Currently, this will be incorrect if you call GetEulerRotation() before SetWorldRotation() or Rotate()!!!!
	} 


	void Transform::SetWorldRotation(vec3 eulerXYZ)
	{
		// Update Quaternion:
		this->worldRotation = glm::quat(eulerXYZ);

		this->rotation = glm::mat4_cast(this->worldRotation);

		UpdateLocalAxis();

		this->eulerWorldRotation = eulerXYZ;
		BoundEulerAngles();

		MarkDirty();
	}


	void Transform::SetWorldRotation(quat newRotation)
	{
		this->worldRotation = newRotation;

		this->rotation = glm::mat4_cast(newRotation);

		UpdateLocalAxis();

		// Update Euler angles:
		this->eulerWorldRotation = glm::eulerAngles(newRotation);
		BoundEulerAngles();

		MarkDirty();
	}


	void Transform::SetWorldScale(vec3 scale)
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

	vec3& Transform::RotateVector(vec3& targetVector, float const& radians, vec3 const& axis)
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

	
	void Transform::Recompute()
	{
		if (!isDirty)
		{
			return;
		}			

		this->model = this->translation * this->scale * this->rotation;

		this->combinedModel			= this->model;
		this->combinedScale			= this->scale;
		this->combinedRotation		= this->rotation;
		this->combinedTranslation	= this->translation;
		if (this->parent != nullptr)
		{
			this->combinedModel			= this->parent->Model(WORLD_MODEL) * combinedModel;
			this->combinedScale			= this->parent->Model(WORLD_SCALE) * combinedScale;
			this->combinedRotation		= this->parent->Model(WORLD_ROTATION) * combinedRotation;
			this->combinedTranslation	= this->parent->Model(WORLD_TRANSLATION) * combinedTranslation;
		}

		this->worldPosition = combinedModel * vec4(0, 0, 0, 1);
		
		// TODO: Recompute eulerWorldRotation
		//this->eulerWorldRotation = ???

		this->worldScale = this->combinedScale * vec4(1, 1, 1, 1);

		for (int i = 0; i < (int)children.size(); i++)
		{
			children.at(i)->MarkDirty();
		}

		isDirty = false;
	}


	void Transform::UpdateLocalAxis()
	{
		// Update the world-space orientation of our local CS axis:
		this->right		= normalize((rotation * vec4(WORLD_X, 0)).xyz());
		this->up		= normalize((rotation * vec4(WORLD_Y, 0)).xyz());
		this->forward	= normalize((rotation * vec4(WORLD_Z, 0)).xyz());
	}


	void Transform::BoundEulerAngles()
	{
		// Keep (signed) Euler xyz angles in (-2pi, 2pi):
		eulerWorldRotation.x = glm::fmod<float>(glm::abs(eulerWorldRotation.x), glm::two_pi<float>()) * glm::sign(eulerWorldRotation.x);
		eulerWorldRotation.y = glm::fmod<float>(glm::abs(eulerWorldRotation.y), glm::two_pi<float>()) * glm::sign(eulerWorldRotation.y);
		eulerWorldRotation.z = glm::fmod<float>(glm::abs(eulerWorldRotation.z), glm::two_pi<float>()) * glm::sign(eulerWorldRotation.z);
	}
}

