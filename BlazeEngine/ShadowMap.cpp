#include "ShadowMap.h"
#include "CoreEngine.h"

//#include "BuildConfiguration.h"


namespace BlazeEngine
{
	ShadowMap::ShadowMap()
	{
		shadowCam		= new Camera("Unnamed_ShadowMapCam");

		renderTexture	= RenderTexture
		(
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapWidth,
			CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultShadowMapHeight
		);

		CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
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
		
		this->renderTexture = RenderTexture
		(
			xRes,
			yRes,
			lightName + "_RenderTexture",
			true
		);

		this->shadowCam->RenderTarget() = &renderTexture;

		CoreEngine::GetSceneManager()->RegisterCamera(CAMERA_TYPE_SHADOW, this->shadowCam);
	}


	//ShadowMap::~ShadowMap()
	//{
	//}
}


