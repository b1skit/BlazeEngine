// Base light class. All other forwardLights inherit from this.
// Defaults as a directional light.

#pragma once

#include "SceneObject.h"

#include "glm.hpp"

using glm::vec3;
using glm::vec4;


namespace BlazeEngine
{
	enum LIGHT_TYPE
	{
		LIGHT_DIRECTIONAL,
		LIGHT_POINT,
		LIGHT_SPOT,
		LIGHT_AREA,
		LIGHT_TUBE,
	};

	class Light : public SceneObject
	{
	public:
		Light(LIGHT_TYPE type, vec4 color, float intensity);

		inline vec4 const& Color() { return color; }
		inline float const& Intensity() { return intensity; }

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

	protected:



	private:
		vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		float intensity = 1.0f;
		LIGHT_TYPE type = LIGHT_DIRECTIONAL;

	};
}