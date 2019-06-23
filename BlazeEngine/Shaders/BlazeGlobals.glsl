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
float GetShadowFactor(vec3 shadowPos, sampler2D shadowMap, vec3 worldNml, vec3 lightDir)
{
	vec3 shadowScreen = (shadowPos.xyz + 1.0) / 2.0; // Projection -> Screen [0,1] space

	// Compute a slope-scaled bias:
	float biasAmount	= max( maxShadowBias * (1.0 - dot(worldNml, lightDir)), minShadowBias);
	float biasedDepth	= shadowScreen.z - biasAmount;

	// Compute a 4x4 block of samples around our fragment, starting at the top-left:
	shadowScreen.x -= 1.5 * shadowDepth_TexelSize.x;
	shadowScreen.y += 1.5 * shadowDepth_TexelSize.y;

	float depthSum = 0;
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			depthSum += (biasedDepth < texture(shadowMap, shadowScreen.xy).r ? 1.0 : 0.0);
			
			shadowScreen.x += shadowDepth_TexelSize.x;
		}

		shadowScreen.x -= 4.0 * shadowDepth_TexelSize.x;
		shadowScreen.y -= shadowDepth_TexelSize.y;
	}

	depthSum /= 16.0;

	return depthSum;
}