#version 430 core

#define BLAZE_FRAGMENT_SHADER
#define BLAZE_VEC4_OUTPUT

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


void main()
{	
	FragColor = texture(GBuffer_Albedo, data.uv0.xy);
} 