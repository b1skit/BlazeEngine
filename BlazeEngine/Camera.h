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

		// Render properties:
		float exposure			= 1.0f;
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
		void Update() {} // Do nothing

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo) {} // Do nothing

		// Getters/Setters:
		inline float const& FieldOfView() const		{ return cameraConfig.fieldOfView; }
		inline float const& Near() const			{ return cameraConfig.near; }
		inline float const& Far() const				{ return cameraConfig.far; }

		mat4 const&			View();
		mat4 const*			CubeView(); // TODO: Recompute this if the camera has moved


		inline mat4 const&	Projection() const		{ return projection; }
		
		inline mat4 const&	ViewProjection()		{ return viewProjection = projection * View(); } // TODO: Only compute this if something has changed
		mat4 const*			CubeViewProjection();

		Material*&			RenderMaterial()		{ return renderMaterial; }

		float& Exposure()							{ return cameraConfig.exposure; }


		// Configure this camera for deferred rendering
		void				AttachGBuffer();

		void				DebugPrint();
	protected:


	private:
		// Helper function: Configures the camera based on the cameraConfig. MUST be called at least once during setup
		void Initialize();

		CameraConfig cameraConfig;

		mat4 view					= mat4();
		mat4 projection				= mat4();
		mat4 viewProjection			= mat4();

		mat4* cubeView				= nullptr;	// Only initialized if we actually need it. Deallocated by Destroy()
		mat4* cubeViewProjection	= nullptr;
		
		Material* renderMaterial	= nullptr;	// Deallocated by Destroy()

		/*bool isDirty = false;*/
	};


}
