#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"
#include "BlazeLighting.glsl"


#ifdef BLIT_IEM

uniform int numSamples;

// Remap from equirectangular to cubemap, performing IEM filtering (ie. for diffuse IBL)
void main()
{	
	// Direction from the center of the cube map towards the current pixel, in "world" space:
	vec3 worldDir   = normalize(data.localPos);

    // Create an orthonormal basis, with worldDir as our "normal"/up:
    vec3 tangent    = normalize(vec3(worldDir.y + 1.0, worldDir.z, worldDir.x)); // Arbitrary: Ensure we don't end up with cross(worldDir, worldDir)
    vec3 bitangent  = normalize(cross(tangent, worldDir));
    tangent         = normalize(cross(worldDir, bitangent));

	vec3 irradiance = vec3(0.0);
	
	// Hammerseley cosine-weighted sampling:
	for (int i = 0; i < numSamples; i++)
	{
		vec2 samplePoints = Hammersley2D(i, numSamples);

		vec3 hemSample = HemisphereSample_cos(samplePoints.x, samplePoints.y); // TODO: MAKE ARG TAKE A VEC2!!!!

		// Project: Tangent space (Z-up) -> World space:
		hemSample = normalize(vec3(dot(hemSample, vec3(tangent.x, bitangent.x, worldDir.x)), dot(hemSample, vec3(tangent.y, bitangent.y, worldDir.y)), dot(hemSample, vec3(tangent.z, bitangent.z, worldDir.z))));

		// Sample the environment:
		vec2 equirectangularUVs	= DirectionToEquirectangularUV(hemSample);
		irradiance				+= texture(albedo, equirectangularUVs).rgb;
	}

	// Simple Monte Carlo approximation of the integral:
	irradiance = irradiance / float(numSamples); // TODO: Should this be  PI * irradiance / float(numSamples); ??

	FragColor = vec4(irradiance, 1.0);
}


#elif defined BLIT_PMREM

// Remap from equirectangular to cubemap, performing PMREM filtering (ie. for specular IBL)
void main()
{	
	// TODO: PMREM blit
}


#else


// Remap from equirectangular to cubemap, with no processing/filtering (ie. for using HDR images as a skybox texture)
void main()
{	
	vec3 worldDir   = normalize(data.localPos);

	vec2 equirectangularUVs	= DirectionToEquirectangularUV(worldDir);
	
	FragColor = vec4(texture(albedo, equirectangularUVs).rgb, 1.0);
}

#endif