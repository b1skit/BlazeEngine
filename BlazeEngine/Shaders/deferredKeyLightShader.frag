#version 430 core

#define BLAZE_FRAGMENT_SHADER
#define BLAZE_VEC4_OUTPUT

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
	FragColor				= texture(GBuffer_Albedo, data.uv0.xy); // Note: For PBR, we require all calculations to be performed in linear color
	vec3 worldNormal		= texture(GBuffer_WorldNormal, data.uv0.xy).xyz;
	vec4 RMAO				= texture(GBuffer_RMAO, data.uv0.xy);
	vec4 worldPosition		= texture(GBuffer_WorldPos, data.uv0.xy);
	vec4 matProp0			= texture(GBuffer_MatProp0, data.uv0.xy);	// .rgb = F0 (Surface response at 0 degrees), .a = Phong exponent

	// Read from 2D shadow map:
	float NoL				= max(0.0, dot(worldNormal, keylightWorldDir));
	vec3 shadowPos			= (shadowCam_vp * worldPosition).xyz;
	float shadowFactor		= GetShadowFactor(shadowPos, shadowDepth, NoL);

	// Note: Keylight lightColor doesn't need any attenuation to be factored in
	FragColor = ComputePBRLighting(FragColor, worldNormal, RMAO, worldPosition, matProp0.rgb, NoL, keylightWorldDir, keylightViewDir, lightColor, shadowFactor, in_view);
} 