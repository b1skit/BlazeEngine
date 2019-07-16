#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


void main()
{	
	// Phong diffuse and specular contributions:
	FragColor					= texture(GBuffer_Albedo, data.uv0.xy);

	vec3 worldNormal			= texture(GBuffer_WorldNormal, data.uv0.xy).xyz;


	// Diffuse:
	float nDotL					= max(0, dot(worldNormal, lightWorldDir));
	vec4 diffuseContribution	= FragColor * vec4(nDotL * lightColor, 1);

	// Specular:
	vec4 RMAO					= texture(GBuffer_RMAO, data.uv0.xy);
	float roughness				= RMAO.r;

	vec3 reflectDir				= normalize(reflect(-lightWorldDir, worldNormal));		// World-space reflection dir
	reflectDir					= normalize((in_view * vec4(reflectDir, 0))).xyz;		// World -> view space


	vec4 worldPosition			= texture(GBuffer_WorldPos, data.uv0.xy);
	vec4 viewPosition			= in_view * worldPosition;
	vec3 viewDir				= normalize(-viewPosition.xyz);
	

	float vDotR					= max(0, dot(viewDir, reflectDir));
	
	float phongExponent			= texture(GBuffer_MatProp0, data.uv0.xy).r; // .r = Phong exponent

	vec4 specContribution		= vec4(roughness * lightColor * pow(vDotR, phongExponent), 0);

	// Shadow:
	vec3 shadowPos				= (shadowCam_vp * worldPosition).xyz;

	float shadowFactor			= GetShadowFactor(shadowPos, shadowDepth, worldNormal, lightWorldDir);

	// Final result:
	FragColor = ((diffuseContribution + specContribution) * shadowFactor);
} 