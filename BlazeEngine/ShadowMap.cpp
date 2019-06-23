#include "ShadowMap.h"
#include "CoreEngine.h"

//#include "BuildConfiguration.h"


namespace BlazeEngine
{
	ShadowMap::ShadowMap()
	{
		this->shadowCam		= new Camera("Unnamed_ShadowMapCam");

		RenderTexture* depthRenderTexture = new RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapWidth,
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapHeight
		);

		InitializeShadowCam(depthRenderTexture);
	}


	ShadowMap::ShadowMap(string lightName, int xRes, int yRes, CameraConfig shadowCamConfig, Transform* shadowCamParent /*= nullptr*/, vec3 shadowCamPosition /* = vec3(0.0f, 0.0f, 0.0f)*/)
	{
		this->shadowCam = new Camera(lightName + "_ShadowMapCam", shadowCamConfig, shadowCamParent);
		this->shadowCam->GetTransform()->SetPosition(shadowCamPosition);

		RenderTexture* depthRenderTexture = new RenderTexture // Deallocated by Camera.Destroy()
		(
			xRes,
			yRes,
			lightName + "_RenderTexture",
			true
		);

		InitializeShadowCam(depthRenderTexture);
	}


	void ShadowMap::InitializeShadowCam(RenderTexture* renderTexture)
	{
		this->shadowCam->RenderMaterial() = new Material(shadowCam->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->shader.depthShaderName);
		this->shadowCam->RenderMaterial()->AccessTexture(RENDER_TEXTURE_DEPTH) = renderTexture;

		CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
	}
}


