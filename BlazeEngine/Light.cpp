#include "Light.h"

namespace BlazeEngine
{
	BlazeEngine::Light::Light(LIGHT_TYPE type, vec4 color, float intensity)
	{
		this->type = type;
		this->color = color;
		this->intensity = intensity;
	}

	void Light::Update()
	{
	}

	void Light::HandleEvent(EventInfo const * eventInfo)
	{
	}
}

