#pragma once

#include "SceneObject.h"


#define GLM_FORCE_SWIZZLE
#include "glm.hpp"


namespace BlazeEngine
{
	class Camera : public SceneObject
	{
	public:
		// Default constructor
		Camera(string cameraName);

		Camera(string cameraName, vec3 position, float fieldOfView, float near, float far, float aspectRatio, Transform* parent = nullptr, bool isOrthographic = false, float orthoWidthHalfRes = 5, float orthoHeightHalfRes = 5);

		/*~Camera();*/

		void Initialize(float aspectRatio, float fieldOfView, float near, float far, Transform* parent = nullptr, vec3 position = vec3(0.0f, 0.0f, 0.0f), bool isOrthographic = false, float orthoWidthHalfRes = 5, float orthoHeightHalfRes = 5);

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline float const& FieldOfView() const		{ return fieldOfView; }
		inline float const& Near() const			{ return near; }
		inline float const& Far() const				{ return far; }

		mat4 const&			View();

		inline mat4 const& Projection() const		{ return projection; }

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
		float aspectRatio	= 1.0f;				// == width / height

		mat4 view;
		mat4 projection;
		mat4 viewProjection;

		bool isOrthographic		= false;
		int orthoHalfWidth		= 5;
		int orthoHalfHeight		= 5;
		
		/*bool isDirty = false;*/
	};


}
