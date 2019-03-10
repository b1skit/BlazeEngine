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

		mat4 Model();
		
		// Hierarchy functions:
		inline Transform* GetParent() const { return parent; }
		void SetParent(Transform* parent);
		void UnParent();
		

		// Functionality:
		void Translate(vec3 amount);
		void SetPosition(vec3 position);

		void Rotate(vec3 eulerXYZ); // Euler rotation. Rotation order = x,y,z
		inline vec3 const& GetEulerRotation() { return eulerRotation; }
		/*void SetRotation(quat rotation);*/
		

		/*void SetScale(vec3 scale);*/
		//void Scale(vec3 amount, SPACE space);

		// Getters/Setters:
		inline vec3 const& Forward() const { return forward; }
		inline vec3 const& Right() const { return right; }
		inline vec3 const& Up() const { return up; }

		// Camera-relative (RHCS) static constants:
		static const vec3 WORLD_RIGHT;		// +X
		static const vec3 WORLD_UP;			// +Y
		static const vec3 WORLD_FORWARD;	// -Z
		

	protected:
		// Helper functions for SetParent()/Unparent():
		void RegisterChild(Transform* child);
		void UnregisterChild(Transform const* child);


	private:
		Transform* parent = nullptr;
		vector<Transform*> children;

		vec3 position		= vec3(0.0f, 0.0f, 0.0f);
		vec3 eulerRotation	= vec3(0.0f, 0.0f, 0.0f);	// Current Euler angles about the local axis, in Radians
		
		// Local CS axis: OpenGL == RHCS in object/world space, LHCS in window (screen) space
		vec3 forward	= vec3(0.0f, 0.0f, -1.0f);
		vec3 right		= vec3(1.0f, 0.0f, 0.0f);
		vec3 up			= vec3(0.0f, 1.0f, 0.0f);

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


