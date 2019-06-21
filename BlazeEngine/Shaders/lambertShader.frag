#version 430 core
#define BLAZE_FRAGMENT_SHADER
#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


void main()
{	
	FragColor = texture(albedo, data.uv0.xy);

	float nDotL = max(0, dot(data.vertexWorldNormal, key_direction));

	float shadowFactor = GetShadowFactor(data.worldPos, key_vp, shadowDepth, data.vertexWorldNormal, key_direction); // TEMP: Pass key direction directly... Should be passing generic light's dir
	
	FragColor = (FragColor * vec4(ambient, 1) ) + (FragColor * vec4(nDotL * key_color, 1) * shadowFactor );
} 