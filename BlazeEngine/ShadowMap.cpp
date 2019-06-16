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


	// Orthographic constructor:
	ShadowMap::ShadowMap(string lightName, int xRes, int yRes, float near, float far, Transform* parent /*= nullptr*/, vec3 position /*= vec3(0.0f, 0.0f, 0.0f)*/, float orthoLeft /*= -5*/, float orthoRight /*= 5*/, float orthoBottom /*= -5*/, float orthoTop /*= 5*/)
	{
		this->shadowCam = new Camera
		(
			lightName + "_ShadowMapCam",
			near,
			far,
			parent,
			position,
			orthoLeft,
			orthoRight,
			orthoBottom, 
			orthoTop
		);		
		
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
		this->shadowCam->RenderMaterial() = new Material(shadowCam->GetName() + "_Material", CoreEngine::GetCoreEngine()->GetConfig()->shader.depthShaderName, true);
		this->shadowCam->RenderMaterial()->SetTexture(renderTexture, RENDER_TEXTURE_DEPTH);

		CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
	}
}


