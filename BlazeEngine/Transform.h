// Game object transformation component

#pragma once

#define GLM_FORCE_SWIZZLE // Enable swizzle operators
#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include <vector>

using glm::vec3;
using glm::vec4;
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

		// Get the model matrix, used to transform from local->world space
		mat4 Model();

		// Get the model rotation matrix (ie. for transforming normals from local -> world space)
		mat4 ModelRotation();

		//// Set the matrices used to compose the model matrix
		//void SetModelMatrices(mat4 translation, mat4 scale, mat4 rotation);
		
		// Hierarchy functions:
		inline Transform* GetParent() const { return parent; }
		void SetParent(Transform* parent);
		void UnParent();
		
		// Functionality:
		//---------------

		// Translate, in (relative) world space
		void Translate(vec3 amount);

		// Set the position, in (relative) world space
		void SetPosition(vec3 position);
		void LookAt(vec3 camForward, vec3 camUp);

		// Get the position, in (relative) world space
		inline vec3 const& Position() { return position; }

		// Rotate about the world X, Y, Z axis, in that order
		// eulerXYZ = Rotation angles about each axis, in RADIANS
		void Rotate(vec3 eulerXYZ);

		inline vec3 const& GetEulerRotation() { return eulerRotation; }
		void SetEulerRotation(vec3 eulerXYZ);
		
		// Set the 
		void SetScale(vec3 scale);


		// Getters/Setters:
		//-----------------

		// Transform's world-space forward (Z+) vector
		inline vec3 const& Forward() const { return forward; }

		// Transform's world-space right (X+) vector
		inline vec3 const& Right() const { return right; }

		// Transform's world-space up (Y+) vector
		inline vec3 const& Up() const { return up; }


		// World CS axis: BlazeEngine always uses a RHCS
		static const vec3 WORLD_X;		// +X
		static const vec3 WORLD_Y;		// +Y
		static const vec3 WORLD_Z;		// +Z
		
		// Print debug information for this transform. Does nothing if DEBUG_LOG_OUTPUT is undefined in BuildConfiguration.h
		void DebugPrint();

		// Static helper functions:
		//-------------------------
		// Rotate a targetVector about an axis by radians
		static vec3& RotateVector(vec3& targetVector, float const & radians, vec3 const & axis);


	protected:
		// Helper functions for SetParent()/Unparent():
		void RegisterChild(Transform* child);
		void UnregisterChild(Transform const* child);


	private:
		Transform* parent = nullptr;
		vector<Transform*> children;

		// World-space orientation:
		vec3 position		= vec3(0.0f, 0.0f, 0.0f);
		vec3 eulerRotation	= vec3(0.0f, 0.0f, 0.0f);	// Current world-space Euler angles (pitch, yaw, roll), in Radians
		vec3 worldScale		= vec3(1.0f, 1.0f, 1.0f);
		
		// Local CS axis: BlazeEngine always uses a RHCS
		vec3 right		= WORLD_X;
		vec3 up			= WORLD_Y;
		vec3 forward	= WORLD_Z;

		/*quat localRotation;
		quat worldRotation;*/

		// model == T*R*S
		mat4 model			= mat4(1.0f);
		mat4 scale			= mat4(1.0f);
		mat4 rotation		= mat4(1.0f);
		mat4 translation	= mat4(1.0f);
		

		/*void Recalculate(); // TO DO: Implement this */
		/*bool isDirty;*/

		
	};
}


