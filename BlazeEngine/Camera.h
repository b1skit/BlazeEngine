#pragma once

#include "SceneObject.h"



#define GLM_FORCE_SWIZZLE
#include "glm.hpp"


namespace BlazeEngine
{
	// Pre-declarations:
	class Material;

	// Contains configuration specific to a cameras rendering
	struct CameraConfig
	{
		// These default values are all overwritten during camera setup

		float fieldOfView		= 90.0f;			// == 0 if orthographic	
		float near				= 1.0f;
		float far				= 100.0f;
		float aspectRatio		= 1.0f;				// == width / height

		// Orthographic rendering properties:
		bool isOrthographic		= false;
		float orthoLeft			= -5;
		float orthoRight		= 5;
		float orthoBottom		= -5;
		float orthoTop			= 5;
	};


	class Camera : public SceneObject
	{
	public:
		// Default constructor
		Camera(string cameraName);

		// Config constructor
		Camera(string cameraName, CameraConfig camConfig, Transform* parent = nullptr);

		void Destroy();

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline float const& FieldOfView() const		{ return cameraConfig.fieldOfView; }
		inline float const& Near() const			{ return cameraConfig.near; }
		inline float const& Far() const				{ return cameraConfig.far; }

		mat4 const&			View();

		inline mat4 const&	Projection() const		{ return projection; }

		inline mat4 const&	ViewProjection()		{ return viewProjection = projection * View(); } // TODO: ONLY COMPUTE THIS MATRIX IF SOMETHING HAS CHANGED!!!

		Material*&			RenderMaterial()		{ return renderMaterial; }


		// Configure this camera for deferred rendering
		void AttachGBuffer();

		void				DebugPrint();
	protected:


	private:
		// Helper function: Configures the camera based on the cameraConfig. MUST be called at least once during setup
		void Initialize();

		CameraConfig cameraConfig;

		mat4 view					= mat4();
		mat4 projection				= mat4();
		mat4 viewProjection			= mat4();
		
		Material* renderMaterial	= nullptr;	// Deallocated when Destroy() is called

		/*bool isDirty = false;*/
	};


}
