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
		Camera() : SceneObject::SceneObject("Unnamed GameObject")
		{
			view = mat4( 1.0f );
			/*projection = mat4(1.0f);*/

			aspectRatio = 800.0f / 600.0f; // TO DO: HAVE THIS CONFIGURED BASED ON THE RESOLUTION!!!!

			projection = glm::perspective(glm::radians(fieldOfView), aspectRatio, near, far);

			viewProjection = mat4(1.0f);

			/*isDirty = false;*/
		}

		/*~Camera();*/

		// BlazeObject interface:
		void Startup(CoreEngine * coreEngine);
		void Shutdown();
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
		float aspectRatio;

		mat4 view;
		mat4 projection;
		mat4 viewProjection;

		/*bool isDirty = false;*/
	};


}
