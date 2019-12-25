// Blaze Engine Lighting Common
// Defines lighting functions common to all shaders

// PBR Lighting:
//--------------

#define PI 3.1415926535897932384626433832795

// Trowbridge-Reitz GGX Normal Distribution Function: Approximate area of surface microfacets aligned with the halfway vector between the light and view dirs
float NDF(vec3 normal, vec3 halfVector, float roughness)
{
	float roughness2	= pow(roughness, 4.0);

	float nDotH			= max(0.0, dot(normal, halfVector));
	float nDotH2		= nDotH * nDotH;

	float denominator	= max((nDotH2 * (roughness2 - 1.0)) + 1.0, 0.0001);
	
	return roughness2 / (PI * denominator * denominator);
}


// Remap roughness for the geometry function, when computing direct lighting contributions
float RemapRoughnessDirect(float roughness)
{
	// Non-linear remap [0,1] -> [0.125, 0.5] (https://www.desmos.com/calculator/mtb0ffbl82)

	float numerator = (roughness + 1.0);
	numerator *= numerator;

	return numerator / 8.0;
}


// Remap roughness for the geometry function, when computing image-based lighting contributions
float RemapRoughnessIBL(float roughness)
{
	// Non-linear remap [0, 1] -> [0, 0.5] (https://www.desmos.com/calculator/top4jswimr)

	float roughness2	= roughness * roughness;

	return roughness2 / 2.0;
}


// Helper function for geometry function
float GeometrySchlickGGX(float NoV, float remappedRoughness)
{
	float nom   = NoV;
	float denom = (NoV * (1.0 - remappedRoughness)) + remappedRoughness;
	
	return nom / denom;
}


// Geometry function: Compute the proportion of microfacets visible
float GeometrySmith(float NoV, float NoL, float remappedRoughness)
{
	float ggx1	= GeometrySchlickGGX(NoV, remappedRoughness);
	float ggx2	= GeometrySchlickGGX(NoL, remappedRoughness);
	
	return ggx1 * ggx2;
}


// Schlick's Approximation: Contribution of Fresnel factor in specular reflection
vec3 FresnelSchlick(float NoV, vec3 F0)
{
	return F0 + ((1.0 - F0) * pow(1.0 - NoV, 5.0));
}


// Compute the halfway vector between light and view dirs
vec3 HalfVector(vec3 light, vec3 view)
{
	vec3 halfVector = light + view;
	halfVector = normalize(halfVector);

	return halfVector;
}


// General PBR lighting: Called from specific deferred light shaders
// FragColor = Non-linearized RGB
// lightWorldDir must be normalized
// lightColor must have attenuation factored in
vec4 ComputePBRLighting(vec4 FragColor, vec3 worldNormal, vec4 RMAO, vec4 worldPosition, vec3 F0, float NoL, vec3 lightWorldDir, vec3 lightViewDir, vec3 lightColor, float shadowFactor, mat4 in_view)
{
	// Convert non-linear RGB to linear:
	FragColor.rgb = pow(FragColor.rgb, vec3(2.2, 2.2, 2.2));

	vec4 viewPosition	= in_view * worldPosition;							// View-space position
	vec3 viewEyeDir		= normalize(-viewPosition.xyz);						// View-space eye/camera direction
	vec3 viewNormal		= normalize(in_view * vec4(worldNormal, 0)).xyz;	// View-space surface normal

	vec3 halfVectorView	= HalfVector(lightViewDir, viewEyeDir);				// View-space half direction
	
	float NoV		= max(0.0, dot(viewNormal, viewEyeDir) );

	float roughness = RMAO.x;
	float metalness = RMAO.y;	

	// Fresnel-Schlick approximation is only defined for non-metals, so we blend it here:
	F0	= mix(F0, FragColor.rgb, metalness); // Linear interpolation: x, y, using t=[0,1]. Returns x when t=0 -> Blends towards albedo for metals

	vec3 fresnel = FresnelSchlick(NoV, F0);
	
	float NDF = NDF(viewNormal, halfVectorView, roughness);

	float remappedRoughness = RemapRoughnessDirect(roughness);
	float geometry			= GeometrySmith(NoV, NoL, remappedRoughness);

	// Specular:
	vec3 specularContribution = (NDF * fresnel * geometry) / max((4.0 * NoV * NoL), 0.0001);
	
	// Diffuse:
	vec3 k_d = vec3(1.0) - fresnel;
	k_d = k_d * (1.0 - metalness); // Metallics absorb refracted light
	vec3 diffuseContribution = k_d * FragColor.rgb; // Note: Omitted the "/ PI" factor here
//	vec3 diffuseContribution = k_d * FragColor.rgb / PI;


	vec3 combinedContribution = diffuseContribution + specularContribution;

	combinedContribution *= lightColor;
	combinedContribution *= NoL;
	
	combinedContribution *= shadowFactor;

	return vec4(combinedContribution, FragColor.a);
}


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
//float GetSlopeScaleBias(vec3 worldNml, vec3 lightDir)
float GetSlopeScaleBias(float NoL)
{
//	return max( maxShadowBias * (1.0 - dot(worldNml, lightDir)), minShadowBias);
	return max( maxShadowBias * (1.0 - NoL), minShadowBias);
}


// Find out if a fragment (in world space) is in shadow
//float GetShadowFactor(vec3 shadowPos, sampler2D shadowMap, vec3 worldNml, vec3 lightDir)
float GetShadowFactor(vec3 shadowPos, sampler2D shadowMap, float NoL)
{
	vec3 shadowScreen = (shadowPos.xyz + 1.0) / 2.0; // Projection -> Screen/UV [0,1] space

	// Compute a slope-scaled bias depth:
//	float biasedDepth	= shadowScreen.z - GetSlopeScaleBias(worldNml, lightDir);
	float biasedDepth	= shadowScreen.z - GetSlopeScaleBias(NoL);

	// Compute a block of samples around our fragment, starting at the top-left:
	const int gridSize = 4; // MUST be a power of two TODO: Compute this on C++ side and allow for uploading of arbitrary samples (eg. odd, even)

	const float offsetMultiplier = (float(gridSize) / 2.0) - 0.5;

	shadowScreen.x -= offsetMultiplier * texelSize.x;
	shadowScreen.y += offsetMultiplier * texelSize.y;

	float depthSum = 0;
	for (int row = 0; row < gridSize; row++)
	{
		for (int col = 0; col < gridSize; col++)
		{
			depthSum += (biasedDepth < texture(shadowMap, shadowScreen.xy).r ? 1.0 : 0.0);
			
			shadowScreen.x += texelSize.x;
		}

		shadowScreen.x -= gridSize * texelSize.x;
		shadowScreen.y -= texelSize.y;
	}

	depthSum /= (gridSize * gridSize);

	return depthSum;
}


// Get shadow factor from a cube map:
//float GetShadowFactor(vec3 lightToFrag, samplerCube shadowMap, vec3 worldNml, vec3 lightDir)
float GetShadowFactor(vec3 lightToFrag, samplerCube shadowMap, float NoL)
{
	float cubemapShadowDepth = texture(shadowMap, lightToFrag).r;
	cubemapShadowDepth *= shadowCam_far;	// [0,1] -> [0, far]

	float fragDepth = length(lightToFrag); // We're using linear depth, for now...

	// Compute a slope-scaled bias:
//	float biasedDepth = fragDepth - GetSlopeScaleBias(worldNml, lightDir);
	float biasedDepth = fragDepth - GetSlopeScaleBias(NoL);

	// TODO: PCF cube map: (jitter the ray)
	// https://www.gamedev.net/forums/topic/674852-pcf-in-cubemap/

	float shadowFactor = 1.0;
	if (biasedDepth > cubemapShadowDepth)
	{
		shadowFactor = 0.0;
	}

	return shadowFactor;
}

	