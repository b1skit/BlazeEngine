#version 430 core
#define BlazeVertexShader
#include "BlazeCommon.glsl"

#define BlazeVertexShader

// Lambert vertex shader
void main()
{
	// Assign position to the predefined gl_Position clip-space output:
    gl_Position = in_mvp * vec4(in_position.x, in_position.y, in_position.z, 1.0);

	data.vertexColor = in_color * vec4(ambient, 1);

	data.fragWorldNormal = (in_model * vec4(in_normal, 0.0f)).xyz;	// Normal -> World normal

	data.uv0 = in_uv0;
}