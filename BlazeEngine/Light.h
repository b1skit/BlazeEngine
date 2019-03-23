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

		NUM_LIGHT_TYPES // Resereved: The number of light types
	};

	class Light : public SceneObject
	{
	public:
		Light(){} // Default constructor
		Light(LIGHT_TYPE type, vec4 color, float intensity);

		inline vec4 const& Color() { return color; }
		inline float const& Intensity() { return intensity; }

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline vec4 const& Color() const { return color; }
		inline void SetColor(vec4 color) { this->color = color; }

		inline float const& Intensity() const { return intensity; }
		inline void SetIntensity(float intensity) { this->intensity = intensity; }

		inline LIGHT_TYPE const& Type() const { return type; }

		inline Transform& GetTransform() { return transform; } // Directional lights shine forward (Z+)

	protected:



	private:
		vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		float intensity = 1.0f;
		LIGHT_TYPE type = LIGHT_DIRECTIONAL;

	};
}