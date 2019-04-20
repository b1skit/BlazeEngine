#include "Light.h"

namespace BlazeEngine
{
	BlazeEngine::Light::Light(string lightName, LIGHT_TYPE type, vec3 color)
	{
		this->lightName = lightName;
		this->type = type;
		this->color = color;
	}

	void Light::Update()
	{
	}

	void Light::HandleEvent(EventInfo const * eventInfo)
	{
	}
}

