#version 430 core

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

in vec4 vertexColor;
in vec3 fragNormal;

uniform vec3 ambient;

uniform vec3 keyDirection;
uniform vec4 keyColor;
uniform float keyIntensity;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_projection;
uniform mat4 in_mv;
uniform mat4 in_mvp;

out vec4 FragColor;

void main()
{
//	FragColor = vertexColor;

	
	FragColor = vertexColor * ambient;

	vec4 nothing = keyColor * keyIntensity * max(0, dot(keyDirection, fragNormal) );

	// Do we need to specify locations for frag shader?
	// How do we link outputs from vert to inputs in frag?
} 