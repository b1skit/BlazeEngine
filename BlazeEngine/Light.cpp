#include "Light.h"
#include "CoreEngine.h"
#include "Camera.h"
#include "BuildConfiguration.h"


namespace BlazeEngine
{
	BlazeEngine::Light::Light(string lightName, LIGHT_TYPE type, vec3 color, ShadowMap* shadowMap /*= nullptr*/)
	{
		this->lightName = lightName;
		this->type = type;
		this->color = color;

		this->shadowMap = shadowMap;
	}


	void Light::Destroy()
	{
		if (shadowMap != nullptr)
		{
			delete shadowMap;
		}

		lightName += "_DELETED";
	}


	void Light::Update()
	{
	}


	void Light::HandleEvent(EventInfo const * eventInfo)
	{
	}

	void Light::AddShadowMap(ShadowMap* newShadowMap)
	{
		if (shadowMap != nullptr)
		{
			LOG("Deleting an existing shadow map");
			delete shadowMap;
		}
		shadowMap = newShadowMap;
	}
}

