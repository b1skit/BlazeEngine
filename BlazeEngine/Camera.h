#pragma once

//#include "CoreEngine.h" // Can't compile if we include this: Circular reference between SceneObject, PlayerObject, Camera ?
#include "SceneObject.h"

#define GLM_FORCE_SWIZZLE
#include "glm.hpp"


namespace BlazeEngine
{
	class Camera : public SceneObject
	{
	public:
		Camera(string cameraName) : SceneObject::SceneObject(cameraName)
		{
			/*fieldOfView		= CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultFieldOfView;
			near			= CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultNear;
			far				= CoreEngine::GetCoreEngine()->GetConfig()->viewCam.defaultFar;
			aspectRatio		= (float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowXRes / (float)CoreEngine::GetCoreEngine()->GetConfig()->renderer.windowYRes;*/

			Initialize(vec3(0,0,0), fieldOfView, aspectRatio, near, far); // Initialize with our default values...

			/*isDirty = false;*/
		}

		Camera(string cameraName, vec3 position, float fieldOfView, float near, float far, float aspectRatio) : SceneObject::SceneObject(cameraName)
		{
			Initialize(position, fieldOfView, aspectRatio, near, far);
		}

		/*~Camera();*/

		void Initialize(vec3 position, float aspectRatio, float fieldOfView, float near, float far);
		void Initialize(float aspectRatio, float fieldOfView, float near, float far);

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline float const& FieldOfView() const { return fieldOfView; }
		inline float const& Near() const { return near; }
		inline float const& Far() const { return far; }

		mat4 const& View();

		inline mat4 const& Projection() const { return projection; }

		inline mat4 const& ViewProjection()
		{
			viewProjection = projection * View(); // TO DO: ONLY COMPUTE THIS IF SOMETHING HAS CHANGED!!!
			return viewProjection; 
		}
		


	protected:


	private:
		float fieldOfView	= 90.0f;
		float near			= 1.0f;
		float far			= 100.0f;
		float aspectRatio	= 1.0f;

		mat4 view;
		mat4 projection;
		mat4 viewProjection;

		/*bool isDirty = false;*/
	};


}
