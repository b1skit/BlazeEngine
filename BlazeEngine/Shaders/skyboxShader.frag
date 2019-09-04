#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
//in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)

//struct gl_DepthRangeParameters
//{
//    float near;
//    float far;
//    float diff;	// far - near
//};
//uniform gl_DepthRangeParameters gl_DepthRange;


void main()
{	
	vec2 uvs		= vec2(gl_FragCoord.x / screenParams.x, gl_FragCoord.y / screenParams.y); // [0, xRes/yRes] -> [0,1]

	// Cull based on depth:
	if (texture( GBuffer_Depth, uvs).r < gl_FragCoord.z)	// Is the GBuffer depth < the screen aligned quad sitting on the far plane?
	{
		discard;
	}

	// If we've made it this far, sample the cube map:
	vec4 ndcPosition;
	ndcPosition.xy	= ((2.0 * gl_FragCoord.xy) / screenParams.xy) - 1.0;
	
	ndcPosition.z	= ((2.0 * gl_FragCoord.z) - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.diff);
	
	
	ndcPosition.w	= 1.0;

	vec4 clipPos	= ndcPosition / gl_FragCoord.w;
	
	vec4 worldPos	= in_inverse_vp * clipPos;
	worldPos.z *= -1; // Correct our Z

	FragColor = texture(CubeMap_0_Right, worldPos.xyz);
} 