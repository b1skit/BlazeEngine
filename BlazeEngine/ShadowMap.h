#pragma once

#include "RenderTexture.h"

#include "glm.hpp"

using glm::vec3;

#define DEFAULT_SHADOWMAP_TEXPATH	"ShadowMap"		// Shadow maps don't have a filepath...
#define DEFAULT_SHADOWMAP_COLOR		vec4(1,1,1,1)	// Default to white (max far)


namespace BlazeEngine
{
	// Pre-declarations:
	class Camera;
	class Transform;
	struct CameraConfig;

	class ShadowMap
	{
	public:

		// Default constructor (perspective shadowcam):
		ShadowMap();

		ShadowMap(string lightName, int xRes, int yRes, CameraConfig shadowCamConfig, Transform* shadowCamParent = nullptr, vec3 shadowCamPosition = vec3(0.0f, 0.0f, 0.0f));

		// Get the current shadow camera
		inline Camera* ShadowCamera()		{ return shadowCam; }

		inline float const& MaxShadowBias()	{ return maxShadowBias; }
		inline float const& MinShadowBias() { return minShadowBias; }

	protected:


	private:
		Camera*			shadowCam		= nullptr;	// Registed in the SceneManager's currentScene, & deallocated when currentScene calls ClearCameras()

		// Helper function: Init the shadow cam's material, register it, etc
		void InitializeShadowCam(RenderTexture* renderTexture);

		float maxShadowBias				= 0.005f;	// Small offset for when we're making shadow comparisons
		float minShadowBias				= 0.0005f;
	};
}


