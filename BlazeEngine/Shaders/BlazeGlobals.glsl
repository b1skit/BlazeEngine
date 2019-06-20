// Blaze Engine Shader Globals
// Defines functions common to all shaders
//----------------------------------------

mat3 AssembleTBN(vec3 tangent, vec3 bitangent)
{
	vec3 faceNormal = normalize(cross(tangent, bitangent));
	return mat3(tangent, bitangent, faceNormal);
}


// Convert a normal sampled from a texture to an object-space normal
vec3 ObjectNormalFromTexture(mat3 TBN, vec3 textureNormal)
{
	textureNormal	= normalize((textureNormal * 2.0) - 1.0);	// Transform [0,1] -> [-1,1]

	vec3 result		= normalize(TBN * textureNormal);

	return result;
}

// Find out if a fragment (in world space) is in shadow
float GetShadowFactor(vec3 worldPos, mat4 light_vp, sampler2D shadowMap)
{
	vec4 shadowPos = key_vp * vec4(worldPos.xyz, 1);

	vec3 shadowScreen = (shadowPos.xyz + 1.0) / 2.0;

	return ((shadowScreen.z - key_shadowBias) < texture(shadowMap, shadowScreen.xy).r ? 1.0 : 0.0);

//	// TO DO: Return a float in [0,1] for how "in shadow" a fragment is
//	return 1.0;
}