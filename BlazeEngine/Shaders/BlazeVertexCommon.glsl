// Blaze Engine Vertex Shader Common
// Defines variables, structures, and functions common to all vertex shaders

in vec3 in_position;
in vec4 in_color;

in vec3 in_normal;
in vec3 in_tangent;
in vec3 in_bitangent;

in vec2 in_uv0;
//in vec2 in_uv1;
//in vec2 in_uv2;
//in vec2 in_uv3;


// NOTE: For now, this struct must be exactly the same as the one in the frag shader. 
// TO DO: Implement shader #includes...
out struct VtoF
{
	vec4 vertexColor;
	
	vec3 fragWorldNormal;
	vec3 tangent;
	vec3 bitangent;

	vec2 uv0;

//	vec3 worldPos;
	vec3 viewPos;		// Camera/eye-space position
} data;
