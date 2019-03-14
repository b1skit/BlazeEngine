#version 330 core

in vec4 vertexColor;

out vec4 FragColor;

void main()
{
    //FragColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	FragColor = vertexColor;
} 