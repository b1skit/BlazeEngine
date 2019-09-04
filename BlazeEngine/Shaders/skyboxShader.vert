#version 430 core

#define BLAZE_VERTEX_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"


// Phong vertex shader
void main()
{
	gl_Position		= vec4(in_position, 1);	// Our screen aligned quad is already in clip space

	data.uv0		= in_uv0;
}