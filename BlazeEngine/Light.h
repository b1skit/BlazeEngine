// Base light class. All other forwardLights inherit from this.
// Defaults as a directional light.

#pragma once

#include "SceneObject.h"
#include "ShadowMap.h"

#include "glm.hpp"

#include <string>

using glm::vec3;
using glm::vec4;


namespace BlazeEngine
{
	// Pre-declarations:
	class Camera;
	class Material;
	class Mesh;

	enum LIGHT_TYPE
	{
		LIGHT_AMBIENT_COLOR,
		LIGHT_AMBIENT_IBL,
		LIGHT_DIRECTIONAL,
		LIGHT_POINT,
		LIGHT_SPOT,
		LIGHT_AREA,
		LIGHT_TUBE,

		LIGHT_TYPE_COUNT // Resereved: The number of light types
	};


	class Light : public SceneObject
	{
	public:
		Light() {}; // Default constructor
		Light(string lightName, LIGHT_TYPE lightType, vec3 color, ShadowMap* shadowMap = nullptr, float radius = 1.0f);

		void Destroy();

		// BlazeObject interface:
		void Update();

		// EventListener interface:
		void HandleEvent(EventInfo const* eventInfo);

		// Getters/Setters:
		inline vec3 const&			Color() const							{ return color; }
		inline void					SetColor(vec3 color)					{ this->color = color; }

		inline LIGHT_TYPE const&	Type() const							{ return type; }

		inline Transform&			GetTransform()							{ return transform; }	// Directional lights shine forward (Z+)

		inline string const&		Name() const							{ return lightName; }
		
		ShadowMap*&					ActiveShadowMap(ShadowMap* newShadowMap = nullptr);				// Get/set the current shadow map

		inline Mesh*&		DeferredMesh()									{ return deferredMesh; }
		inline Material*&	DeferredMaterial()								{ return deferredMaterial; }


	protected:


	private:
		vec3 color					= vec3(0.0f, 0.0f, 0.0f);	// Note: Intensity is factored into these values
		LIGHT_TYPE type				= LIGHT_DIRECTIONAL;		// Default

		string lightName			= "unnamed_directional_light";

		ShadowMap* shadowMap		= nullptr;							// Deallocated by calling Destroy() during SceneManager.Shutdown()

		// Deferred light setup:
		Mesh* deferredMesh			= nullptr;
		Material* deferredMaterial	= nullptr;
	};
}