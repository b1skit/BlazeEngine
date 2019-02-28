#version 330 core

layout (location = 0) // Set the location of the position input variable
in vec3 position;

void main()
{
	// Assign our position data to the predefined gl_Position output
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
}