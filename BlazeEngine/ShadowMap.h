#pragma once

#include <string>

#include "glm.hpp"

using glm::vec3;
using std::string;


#define DEFAULT_SHADOWMAP_TEXPATH	"ShadowMap"		// Shadow maps don't have a filepath...
#define DEFAULT_SHADOWMAP_COLOR		vec4(1,1,1,1)	// Default to white (max far)


namespace BlazeEngine
{
	// Pre-declarations:
	class Camera;
	class Transform;
	struct CameraConfig;
	class RenderTexture;

	class ShadowMap
	{
	public:

		// Default constructor (perspective shadowcam):
		ShadowMap();

		ShadowMap(string lightName, int xRes, int yRes, CameraConfig shadowCamConfig, Transform* shadowCamParent = nullptr, vec3 shadowCamPosition = vec3(0.0f, 0.0f, 0.0f), bool useCubeMap = false);

		// Get the current shadow camera
		inline Camera* ShadowCamera()		{ return shadowCam; }

		inline float& MaxShadowBias()		{ return maxShadowBias; }
		inline float& MinShadowBias()		{ return minShadowBias; }

	protected:


	private:
		Camera*			shadowCam		= nullptr;	// Registed in the SceneManager's currentScene, & deallocated when currentScene calls ClearCameras()

		// Helper function: Init the shadow cam's material, register it, etc
		void InitializeShadowCam(RenderTexture* renderTexture);

		// TODO: Move these defaults to engine/scene config, and load bias directly from the scene light???
		float maxShadowBias				= 0.005f;	// Small offset for when we're making shadow comparisons
		float minShadowBias				= 0.0005f;
	};
}


