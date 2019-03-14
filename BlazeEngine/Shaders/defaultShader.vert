#version 330 core

layout (location = 0) in vec3 in_position; // Set the location of the position input variable

layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec4 in_color;
layout (location = 3) in vec2 in_uv0;

uniform mat4 in_mvp;

out vec4 vertexColor;

void main()
{
	// Assign our position data to the predefined gl_Position output
    gl_Position = in_mvp * vec4(in_position.x, in_position.y, in_position.z, 1.0);

	vertexColor = in_color;
}