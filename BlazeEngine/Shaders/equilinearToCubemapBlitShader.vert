#version 430 core

#define BLAZE_VERTEX_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"


// Phong vertex shader
void main()
{
	gl_Position		= vec4(in_position, 1);
	data.uv0		= in_uv0;
	data.localPos	= in_position; // Cache our untransformed vertex position

	mat4 rotView = mat4(mat3(in_view)); // remove translation from the view matrix
	vec4 clipPos = in_projection * rotView * vec4(in_position, 1.0);

	gl_Position = clipPos.xyww; // Can we just manually set .z and .w == 1 here, for readability????
}