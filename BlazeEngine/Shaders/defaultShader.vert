#version 430 core

// Define input locations:
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec4 in_color;
layout (location = 3) in vec2 in_uv0;

uniform vec4 ambient;

uniform vec3 keyDirection;	// World space normalized vector pointing towards key light
uniform vec4 keyColor;
uniform float keyIntensity;

uniform mat4 in_model;		// Local -> World
uniform mat4 in_view;		// World -> View
uniform mat4 in_projection; // View -> Projection
uniform mat4 in_mv;			// View * Model
uniform mat4 in_mvp;		// Projection * View * Model

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D roughness;
uniform sampler2D metallic;
uniform sampler2D ambientOcclusion;


out vec4 vertexColor;
out vec3 fragNormal;
out vec2 uv0;

// TO DO: Implement structures to pass through each stage...
//out VS_OUT
//{
//	vec2 uv0;
//} vs_out;

void main()
{
	// Assign our position data to the predefined gl_Position output
    gl_Position = in_mvp * vec4(in_position.x, in_position.y, in_position.z, 1.0);

	// TO DO: Replace gl_Position with an out ^^^ (deprecated!)

	vertexColor = in_color * ambient;

	fragNormal = (in_model * vec4(in_normal, 0.0f)).xyz;	// Normal -> World normal

	float nDotL = max(0, dot(fragNormal, keyDirection));	

	vertexColor += nDotL * keyColor * keyIntensity;

	uv0 = in_uv0;
}