#version 430 core
#define BLAZE_FRAGMENT_SHADER
#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"
#include "BlazeLighting.glsl"

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


void main()
{	
	FragColor					= texture(albedo, data.uv0.xy);
	vec3 worldNormal			= WorldNormalFromTexture(normal, data.uv0.xy, data.TBN);

	// Ambient:
	vec4 ambientContribution	= FragColor * vec4(ambientColor, 1);

	// Diffuse:
	vec4 diffuseContribution	= vec4( LambertianDiffuse(FragColor.xyz, worldNormal, lightColor, lightWorldDir) , 1);

	// Shadow:
	float shadowFactor			= GetShadowFactor(data.shadowPos, shadowDepth, data.vertexWorldNormal, lightWorldDir);

	// Final result:
	FragColor = ambientContribution + (diffuseContribution * shadowFactor);
} 