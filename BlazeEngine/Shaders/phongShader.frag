#version 430 core

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


uniform vec3 ambient;

uniform vec3 keyDirection;	// Normalized, world space, points towards light source
uniform vec3 keyColor;		// Intensity

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
uniform vec3 matProperty0; // .x == Phong cosine exponent
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
	vec3 fragWorldNormal;
	vec3 tangent;
	vec3 bitangent;
	vec2 uv0;

//	vec3 worldPos;
	vec3 viewPos;
} data;

out vec4 FragColor;

void main()
{	
	FragColor = texture(albedo, data.uv0);
	float specColor = texture(RMAO, data.uv0).r;

	// Ambient:
	vec4 ambientContribution = FragColor * vec4(ambient, 1);

	// Diffuse:
	float nDotL = max(0, dot(data.fragWorldNormal, keyDirection));	
	vec4 diffuseContribution = FragColor * vec4(nDotL * keyColor, 1);

	// Specular:
	vec3 viewDir = normalize(-data.viewPos.xyz); // Negate, because camera is looking down Z-
	vec3 reflectDir = reflect(-keyDirection, data.fragWorldNormal);
	reflectDir = (in_view * vec4(reflectDir, 0)).xyz; // World -> view space
	float vDotR = max(0, dot(viewDir, reflectDir));

	float specChannelVal = specColor * pow(vDotR, matProperty0.x);
	vec4 specContribution = vec4(specChannelVal, specChannelVal, specChannelVal, 1);

	// Final result:
	FragColor = ambientContribution + diffuseContribution + specContribution;

//	FragColor = vec4(data.vertexColor.xyz, 1);
//	FragColor = vec4(data.tangent.xyz, 1);
//	FragColor = vec4(data.bitangent.xyz, 1);
} 