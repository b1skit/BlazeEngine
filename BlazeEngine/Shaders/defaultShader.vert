#version 330 core
layout (location = 0) in vec3 aPos;

//attribute vec3 position;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);

	//gl_Position = vec4(position, 1.0);
}