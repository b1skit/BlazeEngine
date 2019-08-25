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


// Compute a depth map bias value based on surface orientation
float GetSlopeScaleBias(vec3 worldNml, vec3 lightDir)
{
	return max( maxShadowBias * (1.0 - dot(worldNml, lightDir)), minShadowBias);
}


// Find out if a fragment (in world space) is in shadow
float GetShadowFactor(vec3 shadowPos, sampler2D shadowMap, vec3 worldNml, vec3 lightDir)
{
	vec3 shadowScreen = (shadowPos.xyz + 1.0) / 2.0; // Projection -> Screen/UV [0,1] space

	// Compute a slope-scaled bias depth:
	float biasedDepth	= shadowScreen.z - GetSlopeScaleBias(worldNml, lightDir);

	// Compute a block of samples around our fragment, starting at the top-left:
	const int gridSize = 4; // MUST be a power of two TODO: Compute this on C++ side and allow for uploading of arbitrary samples (eg. odd, even)

	const float offsetMultiplier = (float(gridSize) / 2.0) - 0.5;

	shadowScreen.x -= offsetMultiplier * GBuffer_Depth_TexelSize.x;
	shadowScreen.y += offsetMultiplier * GBuffer_Depth_TexelSize.y;

	float depthSum = 0;
	for (int row = 0; row < gridSize; row++)
	{
		for (int col = 0; col < gridSize; col++)
		{
			depthSum += (biasedDepth < texture(shadowMap, shadowScreen.xy).r ? 1.0 : 0.0);
			
			shadowScreen.x += GBuffer_Depth_TexelSize.x;
		}

		shadowScreen.x -= gridSize * GBuffer_Depth_TexelSize.x;
		shadowScreen.y -= GBuffer_Depth_TexelSize.y;
	}

	depthSum /= (gridSize * gridSize);

	return depthSum;
}


// Get shadow factor from a cube map:
float GetShadowFactor(vec3 lightToFrag, samplerCube shadowMap, vec3 worldNml, vec3 lightDir)
{
	float cubemapShadowDepth = texture(shadowMap, lightToFrag).r;
	cubemapShadowDepth *= shadowCam_far;	// [0,1] -> [0, far]

	float fragDepth = length(lightToFrag); // We're using linear depth, for now...

	// Compute a slope-scaled bias:
	float biasedDepth = fragDepth - GetSlopeScaleBias(worldNml, lightDir);
	
//	float biasedDepth = fragDepth;

	// TODO: PCF cube map: (jitter the ray)
	// https://www.gamedev.net/forums/topic/674852-pcf-in-cubemap/

	float shadowFactor = 1.0;
	if (biasedDepth > cubemapShadowDepth)
	{
		shadowFactor = 0.0;
	}

	return shadowFactor;
}

	