#version 430 core

#define BLAZE_VERTEX_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"


// Phong vertex shader
void main()
{
	gl_Position = in_mvp * vec4(in_position.xyz, 1.0);

	data.worldPos	= (in_model * vec4(in_position.xyz, 1.0f)).xyz;
	data.viewPos	= -(in_mv * vec4(in_position.xyz, 1.0f)).xyz;	// Negate, because camera is looking down Z-
}