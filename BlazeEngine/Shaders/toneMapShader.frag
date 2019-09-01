#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Gamma = 1.0 / 2.2
#define GAMMA vec3(0.45454545454545454545454545454545454545, 0.45454545454545454545454545454545454545, 0.45454545454545454545454545454545454545)

uniform float exposure = 1.0;	// Uploaded in PostFXManager.Initialize()

void main()
{	
	vec4 color = texture(GBuffer_Albedo, data.uv0.xy);

	vec3 toneMappedColor = vec3(1.0, 1.0, 1.0) - exp(-color.rgb * exposure);

	// Apply Gamma correction:
	toneMappedColor = pow(toneMappedColor, GAMMA);

	FragColor = vec4(toneMappedColor, 1.0);
} 