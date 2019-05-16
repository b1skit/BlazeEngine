#version 430 core
#include "BlazeCommon.glsl"
#include "BlazeVertexCommon.glsl"

// Phong vertex shader
void main()
{
	// Assign position to the predefined gl_Position clip-space output:
    gl_Position = in_mvp * vec4(in_position.x, in_position.y, in_position.z, 1.0);

	data.vertexColor = in_color * vec4(ambient, 1);

	data.fragWorldNormal = (in_model * vec4(in_normal, 0.0f)).xyz;	// Object -> World normal

	data.tangent = in_tangent;		// DEBUG: NEED TO TRANSFORM THESE!!!
	data.bitangent = in_bitangent;

	data.uv0 = in_uv0;

	data.viewPos = (in_mv * vec4(in_position.xyz, 1.0f)).xyz;
}