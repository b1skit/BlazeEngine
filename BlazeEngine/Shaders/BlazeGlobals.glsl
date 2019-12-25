// Blaze Engine Shader Globals
// Defines functions common to all shaders
//----------------------------------------

mat3 AssembleTBN(vec3 localTangent, vec3 localBitangent, mat4 worldRotation)
{
	vec3 worldTangent		= (worldRotation * vec4(localTangent, 0)).xyz;
	vec3 worldBitangent		= (worldRotation * vec4(localBitangent, 0)).xyz;

	vec3 worldFaceNormal	= normalize(cross(worldTangent, worldBitangent));
	
	return mat3(worldTangent, worldBitangent, worldFaceNormal);
}


// Convert a normal sampled from a texture to an object-space normal
vec3 ObjectNormalFromTexture(mat3 TBN, vec3 textureNormal)
{
	textureNormal	= normalize((textureNormal * 2.0) - 1.0);	// Transform [0,1] -> [-1,1]

	vec3 result		= normalize(TBN * textureNormal);

	return result;
}


vec3 WorldNormalFromTexture(sampler2D normal, vec2 uv, mat3 TBN)
{
	vec3 textureNormal	= texture(normal, uv).xyz;
	
	textureNormal		= normalize((textureNormal * 2.0) - 1.0);	// Transform [0,1] -> [-1,1]

	vec3 result			= normalize(TBN * textureNormal);

	return result;
}