#version 430 core

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


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
uniform vec3 matProperty0;
//uniform vec3 matProperty1;
//uniform vec3 matProperty2;
//uniform vec3 matProperty3;
//uniform vec3 matProperty4;
//uniform vec3 matProperty5;
//uniform vec3 matProperty6;
//uniform vec3 matProperty7;


// NOTE: For now, this struct must be exactly the same as the one in the frag shader. 
// TO DO: Implement shader #includes...
in struct VtoF
{
	vec3 vertexColor;
	vec3 fragNormal;
	vec2 uv0;
} data;

out vec4 FragColor;

void main()
{	
	FragColor = texture(albedo, data.uv0);

	float nDotL = max(0, dot(data.fragNormal, keyDirection));

	FragColor = (FragColor * vec4(ambient, 1) ) + (FragColor * vec4(nDotL * keyColor, 1));

//	FragColor = texture(normal, uv0);
//	FragColor = texture(roughness, uv0);
//	FragColor = texture(metallic, uv0);
//	FragColor = texture(ambientOcclusion, uv0);

} 