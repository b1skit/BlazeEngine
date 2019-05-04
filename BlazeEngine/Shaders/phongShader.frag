#version 430 core

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

in vec3 vertexColor;
in vec3 fragNormal;
in vec2 uv0;

uniform vec3 ambient;

uniform vec3 keyDirection;
uniform vec3 keyColor;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_projection;
uniform mat4 in_mv;
uniform mat4 in_mvp;

//layout (location = 0) uniform sampler2D albedo;
//layout (location = 1) uniform sampler2D normal;
//layout (location = 2) uniform sampler2D roughness;
//layout (location = 3) uniform sampler2D metallic;
//layout (location = 4) uniform sampler2D ambientOcclusion;

uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D RMAO;

// Generic material properties:
uniform vec3 matProperty0;		// matProperty.x == cosine power
//uniform vec3 matProperty1;
//uniform vec3 matProperty2;
//uniform vec3 matProperty3;
//uniform vec3 matProperty4;
//uniform vec3 matProperty5;
//uniform vec3 matProperty6;
//uniform vec3 matProperty7;

out vec4 FragColor;


void main()
{	
	// TO DO: IMPLEMENT PHONG SHADING!!!!!!!!!

	FragColor = texture(albedo, uv0);

	float nDotL = max(0, dot(fragNormal, keyDirection));

	FragColor = (FragColor * vec4(ambient, 1) ) + (FragColor * vec4(nDotL * keyColor, 1));

//	FragColor = texture(normal, uv0);
//	FragColor = texture(roughness, uv0);
//	FragColor = texture(metallic, uv0);
//	FragColor = texture(ambientOcclusion, uv0);

} 