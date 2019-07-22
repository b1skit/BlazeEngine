#version 430 core

#define BLAZE_VERTEX_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"


// Phong vertex shader
void main()
{
	gl_Position		= in_mvp * vec4(in_position.xyz, 1.0);
//	data.uv0		= in_uv0; // TODO: Write a screen position to (GBuffer) UV function
}