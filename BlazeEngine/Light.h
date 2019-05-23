// Base light class. All other forwardLights inherit from this.
// Defaults as a directional light.

#pragma once

#include "SceneObject.h"

#include "glm.hpp"

#include <string>

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
		Light() {}; // Default constructor
		Light(string lightName, LIGHT_TYPE type, vec3 color);

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline vec3 const& Color() const			{ return color; }
		inline void SetColor(vec4 color)			{ this->color = color; }

		inline LIGHT_TYPE const& Type() const		{ return type; }

		inline Transform& GetTransform()			{ return transform; } // Directional lights shine forward (Z+)

		inline string const& Name() const			{ return lightName; }

	protected:



	private:
		vec3 color			= vec3(0.0f, 0.0f, 0.0f);	// Intensity is factored into these values
		LIGHT_TYPE type		= LIGHT_DIRECTIONAL;

		string lightName	= "unnamed_light";
	};
}