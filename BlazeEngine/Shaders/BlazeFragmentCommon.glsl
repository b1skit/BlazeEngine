// Blaze Engine Fragment Shader Common
// Defines variables, structures, and functions common to all fragment shaders

in struct VtoF
{
	vec4 vertexColor;
	
	vec3 fragWorldNormal;
	vec3 tangent;
	vec3 bitangent;

	vec2 uv0;

//	vec3 worldPos;
	vec3 viewPos;		// Camera/eye-space position
} data;

out vec4 FragColor;