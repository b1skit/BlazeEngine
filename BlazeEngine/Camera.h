#pragma once

#include "SceneObject.h"


#define GLM_FORCE_SWIZZLE
#include "glm.hpp"


namespace BlazeEngine
{
	// Pre-declarations:
	class Texture;


	class Camera : public SceneObject
	{
	public:
		// Default constructor
		Camera(string cameraName);

		// Perspective constructor
		Camera(string cameraName, float aspectRatio, float fieldOfView, float near, float far, Transform* parent = nullptr, vec3 position = vec3(0.0f, 0.0f, 0.0f));

		// Orthographic constructor:
		Camera(string cameraName, float near, float far, Transform* parent = nullptr, vec3 position = vec3(0.0f, 0.0f, 0.0f), float orthoLeft = -5, float orthoRight = 5, float orthoBottom = -5, float orthoTop = 5);

		/*~Camera();*/

		void Initialize(float aspectRatio, float fieldOfView, float near, float far, Transform* parent = nullptr, vec3 position = vec3(0.0f, 0.0f, 0.0f), bool isOrthographic = false, float orthoLeft = -5, float orthoRight = 5, float orthoBottom = -5, float orthoTop = 5);

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline float const& FieldOfView() const		{ return fieldOfView; }
		inline float const& Near() const			{ return near; }
		inline float const& Far() const				{ return far; }

		mat4 const&			View();

		inline mat4 const&	Projection() const		{ return projection; }

		inline mat4 const&	ViewProjection()		{ return viewProjection = projection * View(); } // TO DO: ONLY COMPUTE THIS IF SOMETHING HAS CHANGED!!!

		Texture*&			RenderTarget()			{ return renderTarget; }

		void				DebugPrint();
	protected:


	private:
		float fieldOfView	= 90.0f;			// == 0 if orthographic	
		float near			= 1.0f;
		float far			= 100.0f;
		float aspectRatio	= 1.0f;				// == width / height

		mat4 view;
		mat4 projection;
		mat4 viewProjection;

		// Orthographic rendering properties:
		bool isOrthographic		= false;
		//float orthoHalfWidth	= 5;			// == 0 if perspective
		//float orthoHalfHeight	= 5;			// == 0 if perspective
		float orthoLeft			= -5;
		float orthoRight		= -5;
		float orthoBottom		= -5;
		float orthoTop			= -5;
		
		Texture* renderTarget	= nullptr;

		/*bool isDirty = false;*/
	};


}
