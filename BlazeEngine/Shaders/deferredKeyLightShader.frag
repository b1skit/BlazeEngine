#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"
#include "BlazeLighting.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


void main()
{
	// Sample textures once inside the main shader flow, and pass the values as required:
	FragColor					= texture(GBuffer_Albedo, data.uv0.xy);
	vec3 worldNormal			= texture(GBuffer_WorldNormal, data.uv0.xy).xyz;
	vec4 RMAO					= texture(GBuffer_RMAO, data.uv0.xy);
	vec4 worldPosition			= texture(GBuffer_WorldPos, data.uv0.xy);
	vec4 matProp0				= texture(GBuffer_MatProp0, data.uv0.xy); // .r = Phong exponent

	// Diffuse:
	vec4 diffuseContribution	= vec4( LambertianDiffuse(FragColor.xyz, worldNormal, lightColor, lightWorldDir) , 1);

	// Specular:
	vec4 specContribution		= vec4( PhongSpecular(worldPosition.xyz, worldNormal, lightWorldDir, lightColor, in_view, RMAO.r, matProp0.r).xyz * FragColor.xyz, 1);
	
	// Shadow:
	vec3 shadowPos				= (shadowCam_vp * worldPosition).xyz;
	float shadowFactor			= GetShadowFactor(shadowPos, shadowDepth, worldNormal, lightWorldDir);


	// Final result:
	FragColor = ((diffuseContribution + specContribution) * shadowFactor);
} 