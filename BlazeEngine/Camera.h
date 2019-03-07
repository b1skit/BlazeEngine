#pragma once

#include "SceneObject.h"

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
			projection = mat4(1.0f);
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
		inline float FieldOfView() const { return fieldOfView; }

		inline mat4 const* View() const { return &view; }
		inline mat4 const* Projection() const { return &projection; }
		inline mat4 const* ViewProjection() const { return &viewProjection; }
		


	protected:


	private:

		float fieldOfView = 90.0f;

		mat4 view;
		mat4 projection;
		mat4 viewProjection;

		/*bool isDirty = false;*/
	};


}
