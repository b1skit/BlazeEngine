#version 430 core

#define BLAZE_FRAGMENT_SHADER
#define BLAZE_GBUFFER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

// Note: Locations must match the order defined in material.h
layout (location = 0) out vec4 gBuffer_out_albedo;
layout (location = 1) out vec4 gBuffer_out_worldNormal;
layout (location = 2) out vec4 gBuffer_out_RMAO;
layout (location = 3) out vec4 gBuffer_out_emissive;
layout (location = 4) out vec4 gBuffer_out_position;
layout (location = 5) out vec4 gBuffer_out_matProp0;
layout (location = 6) out vec4 gBuffer_out_depth;


void main()
{
	// Albedo:
	gBuffer_out_albedo		= texture(albedo, data.uv0.xy);

	// Normal:
	gBuffer_out_worldNormal = vec4( WorldNormalFromTexture(normal, data.uv0.xy, data.TBN), 0);

	// RMAO:
	gBuffer_out_RMAO		= texture(RMAO, data.uv0.xy);

	// Emissive:
	gBuffer_out_emissive	= texture(emissive, data.uv0.xy);

	// Position:
	gBuffer_out_position	= vec4(data.worldPos.xyz, 1); // Homogenize ????

	// Material properties:
	gBuffer_out_matProp0	= matProperty0;

	// Depth:
	gBuffer_out_depth		= vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
}