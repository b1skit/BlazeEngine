#include "ShadowMap.h"
#include "CoreEngine.h"
#include "Camera.h"
#include "RenderTexture.h"
#include "Material.h"
#include "Scene.h"


namespace BlazeEngine
{
	ShadowMap::ShadowMap()
	{
		this->shadowCam		= new Camera("Unnamed_ShadowMapCam");

		RenderTexture* depthRenderTexture = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("defaultShadowMapWidth"),
			CoreEngine::GetCoreEngine()->GetConfig()->GetValue<int>("defaultShadowMapHeight")
		);

		InitializeShadowCam(depthRenderTexture);
	}


	ShadowMap::ShadowMap(string lightName, int xRes, int yRes, CameraConfig shadowCamConfig, Transform* shadowCamParent /*= nullptr*/, vec3 shadowCamPosition /* = vec3(0.0f, 0.0f, 0.0f)*/, bool useCubeMap /*= false*/)
	{
		this->shadowCam = new Camera(lightName + "_ShadowMapCam", shadowCamConfig, shadowCamParent);
		this->shadowCam->GetTransform()->SetWorldPosition(shadowCamPosition);

		// Omni-directional (Cube map) shadowmap setup:
		if (useCubeMap)
		{
			this->shadowCam->RenderMaterial() = new Material(shadowCam->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("cubeDepthShaderName"), CUBE_MAP_NUM_FACES, true);

			RenderTexture** cubeFaces = RenderTexture::CreateCubeMap(xRes, yRes, lightName);

			this->shadowCam->RenderMaterial()->AttachCubeMapTextures((Texture**)cubeFaces);

			// Buffer the cube map:
			RenderTexture::BufferCubeMap(cubeFaces, CUBE_MAP_0);

			CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
		}
		else // Single texture shadowmap setup:
		{
			RenderTexture* depthRenderTexture = new RenderTexture // Deallocated by Camera.Destroy()
			(
				xRes,
				yRes,
				lightName + "_RenderTexture"
			);

			depthRenderTexture->Buffer(RENDER_TEXTURE_0 + RENDER_TEXTURE_DEPTH);

			InitializeShadowCam(depthRenderTexture);
		}		
	}


	// Helper function: Reduces some duplicate code for non-cube map depth textures
	void ShadowMap::InitializeShadowCam(RenderTexture* renderTexture)
	{
		this->shadowCam->RenderMaterial() = new Material(shadowCam->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->GetValue<string>("depthShaderName"), RENDER_TEXTURE_COUNT, true);
		this->shadowCam->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH) = renderTexture;

		CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
	}
}


