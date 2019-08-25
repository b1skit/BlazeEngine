// Blaze Engine Cube Map Depth Shader

#version 430 core

// Set the location of the position input variable
layout (location = 0) in vec3 in_position;

uniform mat4 in_model;

void main()
{
	// Transform to world space:
    gl_Position = in_model * vec4(in_position.xyz, 1.0);

}