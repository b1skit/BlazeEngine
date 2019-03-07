#version 330 core

layout (location = 0) // Set the location of the position input variable
in vec3 in_position;

uniform mat4 in_projection;

void main()
{
	// Assign our position data to the predefined gl_Position output
    gl_Position = in_projection * vec4(in_position.x, in_position.y, in_position.z, 1.0);
	//gl_Position = vec4(in_position.x, in_position.y, in_position.z, 1.0);

	// TO DO: xform local->world->view
}