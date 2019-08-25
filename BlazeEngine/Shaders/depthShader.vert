// Blaze Engine Depth Shader

#version 430 core

// Set the location of the position input variable
layout (location = 0) in vec3 in_position;

uniform mat4 in_mvp;

void main()
{
	// Assign our position data to the predefined gl_Position output
    gl_Position = in_mvp * vec4(in_position.xyz, 1.0);
}