#version 430 core
#include "BlazeCommon.glsl"

// Define input locations:
//layout (location = 0) in vec3 in_position;
//layout (location = 1) in vec3 in_normal;
//layout (location = 2) in vec3 in_color;
//layout (location = 3) in vec2 in_uv0;

in struct Input
{
	vec3 in_position;
	vec3 in_normal;
	vec3 in_color;
	vec2 in_uv0;
} IN;

// NOTE: For now, this struct must be exactly the same as the one in the frag shader. 
// TO DO: Implement shader #includes...
out struct VtoF
{
	vec3 vertexColor;
	vec3 fragNormal;
	vec2 uv0;
} data;


void main()
{
	// Assign position to the predefined gl_Position clip-space output:
    gl_Position = in_mvp * vec4(IN.in_position.x, IN.in_position.y, IN.in_position.z, 1.0);

	data.vertexColor = IN.in_color * ambient;

	data.fragNormal = (in_model * vec4(IN.in_normal, 0.0f)).xyz;	// Normal -> World normal

	data.uv0 = IN.in_uv0;
}