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
		/*inline Transform* GetParent() { return parent; }*/
		/*inline vec3& LocalPosition() { return localPosition; }
		inline vec3& WorldPosition() { return worldPosition; }*/

		/*quat LocalRotation();
		quat WorldRotation();*/

		mat4 Model() const;
		/*inline mat4 const* Model() const;*/ // { return &model; } // Local->World
		/*inline mat4* Model() { return const_cast<mat4*>((*this).Model()); }*/
		
		// Child functions:
		inline Transform const* GetParent() const { return parent; }
		/*inline Transform GetParent(){return const_c}*/
		void SetParent(Transform* parent);
		void UnParent();
		
		
		
		

		/*inline vec3& LocalPosition() { return localPosition; }*/

		// Functionality:
		void Translate(vec3 amount);
		void SetPosition(vec3 position);

		/*void SetRotation(quat rotation);*/
		//void Rotate(quat amount, SPACE space);

		/*void SetScale(vec3 scale);*/
		//void Scale(vec3 amount, SPACE space);

		// Getters/Setters:
		inline vec3 const& Forward() const { return forward; }
		inline vec3 const& Right() const { return right; }
		inline vec3 const& Up() const { return up; }
		

	protected:
		// Helper functions for SetParent()/Unparent():
		void RegisterChild(Transform const* child);
		void UnregisterChild(Transform const* child);


	private:
		Transform* parent = nullptr;
		vector<Transform const*> children;

		vec3 position;
		
		vec3 forward	= vec3(0.0f, 0.0f, -1.0f);	// OpenGL is right handed in object/world space, left handed in window (aka screen) space
		vec3 right		= vec3(1.0f, 0.0f, 0.0f);
		vec3 up			= vec3(0.0f, 1.0f, 0.0f);

		/*quat localRotation;
		quat worldRotation;*/

		mat4 model = mat4( 1.0f );

		/*void Recalculate(); // TO DO: Implement this */
		/*bool isDirty;*/

		
	};
}


