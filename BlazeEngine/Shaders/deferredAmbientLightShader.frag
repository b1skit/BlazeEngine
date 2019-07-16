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
	// Phong ambient contribution:
	FragColor					= texture(GBuffer_Albedo, data.uv0.xy) * vec4(lightColor, 1);
} 