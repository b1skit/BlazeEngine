// Blaze Engine Vertex Shader Common
// Defines variables, structures, and functions common to all vertex shaders

in vec3 in_position;
in vec4 in_color;

in vec3 in_normal;
in vec3 in_tangent;
in vec3 in_bitangent;

in vec4 in_uv0;
in vec4 in_uv1;
in vec4 in_uv2;
in vec4 in_uv3;


out struct VtoF
{
	vec4 vertexColor;
	
	vec3 fragWorldNormal;
	vec3 tangent;
	vec3 bitangent;

	vec4 uv0;
	vec4 uv1;
	vec4 uv2;
	vec4 uv3;

//	vec3 worldPos;
	vec3 viewPos;		// Camera/eye-space position
} data;
