#pragma once

#include "SceneObject.h"
#include "glm.hpp"

// Predeclaration:
class CoreEngine;


namespace BlazeEngine
{
	class Camera : public SceneObject
	{
	public:
		Camera() : SceneObject::SceneObject("Camera Object")
		{
			//aspectRatio = 800.0f / 600.0f; // TO DO: HAVE THIS CONFIGURED BASED ON THE RESOLUTION!!!!

			/*projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, near, far);
			
			view = mat4(1.0f);

			viewProjection = projection * view;*/

			Initialize(vec3(0,0,0), fieldOfView, aspectRatio, near, far); // Initialize with our default values...

			/*isDirty = false;*/
		}

		/*~Camera();*/

		void Initialize(vec3 position, float aspectRatio, float fieldOfView, float near, float far);

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline float const& FieldOfView() const { return fieldOfView; }
		inline float const& Near() const { return near; }
		inline float const& Far() const { return far; }

		mat4 const& View()
		{
			view = inverse(transform.Model());
			return view;
		}

		inline mat4 const& Projection() const { return projection; }

		inline mat4 const& ViewProjection()
		{
			viewProjection = projection * View(); // TO DO: ONLY COMPUTE THIS IF SOMETHING HAS CHANGED!!!
			return viewProjection; 
		}
		


	protected:


	private:

		float fieldOfView = 90.0f;
		float near = 1.0f;
		float far = 100.0f;
		float aspectRatio = 1.0f;

		mat4 view;
		mat4 projection;
		mat4 viewProjection;

		/*bool isDirty = false;*/
	};


}
