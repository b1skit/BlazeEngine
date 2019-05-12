#version 430 core

uniform vec3 ambient;

uniform vec3 keyDirection;	// Normalized, world space, points towards light source
uniform vec3 keyColor;

uniform mat4 in_model;		// Local -> World
uniform mat4 in_view;		// World -> View
uniform mat4 in_projection; // View -> Projection
uniform mat4 in_mv;			// View * Model
uniform mat4 in_mvp;		// Projection * View * Model

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D RMAO;

// Generic material properties:
uniform vec3 matProperty0;
//uniform vec3 matProperty1;
//uniform vec3 matProperty2;
//uniform vec3 matProperty3;
//uniform vec3 matProperty4;
//uniform vec3 matProperty5;
//uniform vec3 matProperty6;
//uniform vec3 matProperty7;

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
	vec3 fragWorldNormal;
	vec2 uv0;

//	vec3 worldPos;
	vec3 viewPos;		// Camera/eye-space position
} data;


void main()
{
	// Assign position to the predefined gl_Position clip-space output:
    gl_Position = in_mvp * vec4(IN.in_position.x, IN.in_position.y, IN.in_position.z, 1.0);

	data.vertexColor = IN.in_color * ambient;

	data.fragWorldNormal = (in_model * vec4(IN.in_normal, 0.0f)).xyz;	// Object -> World normal

	data.uv0 = IN.in_uv0;

	data.viewPos = (in_mv * vec4(IN.in_position.xyz, 1.0f)).xyz;
}