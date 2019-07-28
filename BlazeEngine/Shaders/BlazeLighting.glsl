// Blaze Engine Lighting Common
// Defines lighting functions common to all shaders


// Phong Lighting
//---------------

// Compute the diffuse contribution:
vec3 LambertianDiffuse(vec3 fragColor, vec3 worldNormal, vec3 lightColor, vec3 lightWorldDir)
{
	float nDotL					= max(0.0, dot(worldNormal, lightWorldDir));
	vec3 diffuseContribution	= fragColor * nDotL * lightColor;

	return diffuseContribution;
}


vec3 PhongSpecular(vec3 worldPosition, vec3 worldNormal, vec3 lightWorldDir, vec3 lightColor, mat4 view, float specMask, float phongExponent)
{
	// NOTE: The spec value returned by this function is tinted by the light color only. The surface color is not considered.

	vec3 reflectDir				= normalize(reflect(-lightWorldDir, worldNormal));		// World-space reflection dir
	reflectDir					= normalize((view * vec4(reflectDir, 0))).xyz;		// World -> view space

	vec4 viewPosition			= view * vec4(worldPosition, 1);
	vec3 viewDir				= normalize(-viewPosition.xyz);

	float positiveOffset		= 0.0001; // Avoid 0^0 errors when computing specContribution	

	float vDotR					= dot(viewDir, reflectDir);

	return lightColor * pow(max(positiveOffset, vDotR), phongExponent) * specMask;
}


// Calculate attenuation based on distance between fragment and light
float LightAttenuation(vec3 fragWorldPosition, vec3 lightWorldPosition)
{
	float lightDist = length(lightWorldPosition - fragWorldPosition);

	float attenuation = 1.0 / (1.0 + (lightDist * lightDist));

	return attenuation;
}


// Find out if a fragment (in world space) is in shadow
float GetShadowFactor(vec3 shadowPos, sampler2D shadowMap, vec3 worldNml, vec3 lightDir)
{
	vec3 shadowScreen = (shadowPos.xyz + 1.0) / 2.0; // Projection -> Screen [0,1] space

	// Compute a slope-scaled bias:
	float biasAmount	= max( maxShadowBias * (1.0 - dot(worldNml, lightDir)), minShadowBias);
	float biasedDepth	= shadowScreen.z - biasAmount;

	// Compute a 4x4 block of samples around our fragment, starting at the top-left:
	shadowScreen.x -= 1.5 * GBuffer_Depth_TexelSize.x;
	shadowScreen.y += 1.5 * GBuffer_Depth_TexelSize.y;

	float depthSum = 0;
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 4; col++)
		{
			depthSum += (biasedDepth < texture(shadowMap, shadowScreen.xy).r ? 1.0 : 0.0);
			
			shadowScreen.x += GBuffer_Depth_TexelSize.x;
		}

		shadowScreen.x -= 4.0 * GBuffer_Depth_TexelSize.x;
		shadowScreen.y -= GBuffer_Depth_TexelSize.y;
	}

	depthSum /= 16.0;

	return depthSum;
}