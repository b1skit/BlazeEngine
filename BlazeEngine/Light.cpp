#include "Light.h"
#include "CoreEngine.h"
#include "Camera.h"

namespace BlazeEngine
{
	BlazeEngine::Light::Light(string lightName, LIGHT_TYPE type, vec3 color, bool hasShadow /*= false*/)
	{
		this->lightName = lightName;
		this->type = type;
		this->color = color;

		if (hasShadow)
		{
			shadowCam = new Camera(this->lightName + "_ShadowCamera");
			shadowCam->Initialize
			(
				1.0f,
				1.0f,
				CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultNear, 
				CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultFar, 
				&(this->transform), 
				vec3(0, 0, 0), 
				true, 
				CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultOrthoHalfWidth,	// TO DO: Set this based on a function of shadow texture res and scene dimensions!
				CoreEngine::GetCoreEngine()->GetConfig()->shadows.defaultOrthoHalfHeight	// Repace CoreEngine defaults...
			);
		}
	}

	void Light::Update()
	{
	}

	void Light::HandleEvent(EventInfo const * eventInfo)
	{
	}
}

