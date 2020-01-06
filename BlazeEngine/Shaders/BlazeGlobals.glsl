#ifndef BLAZE_GLOBALS
#define BLAZE_GLOBALS

// Blaze Engine Shader Globals
// Defines functions common to all shaders
//----------------------------------------



// Global defines:
//----------------
#define PI 3.1415926535897932384626433832795

// Gamma = 1.0 / 2.2:
#define GAMMA vec3(0.45454545454545454545454545454545454545, 0.45454545454545454545454545454545454545, 0.45454545454545454545454545454545454545)


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


// Convert a (normalized) view/cubemap direction to an equirectangular UV coordinate. Used to convert HDR maps to cubemaps
vec2 DirectionToEquirectangularUV(vec3 view)
{
	vec2 uv = vec2(atan(view.z, view.x), asin(view.y));
//    uv *= invAtan;
	uv *= vec2(0.1591, 0.3183);
    uv += 0.5;
    
	return uv;
}


// Linearize an sRGB gamma curved color value
vec3 Degamma(vec3 sRGB)
{
	return pow(sRGB, vec3(2.2, 2.2, 2.2));
}


// Apply Gamma correction to a linear color value
vec3 Gamma(vec3 linearColor)
{
	return pow(linearColor, GAMMA);
}


#endif