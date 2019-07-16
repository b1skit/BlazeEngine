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
	FragColor			= texture(albedo, data.uv0.xy);

	float nDotL			= max(0, dot(data.vertexWorldNormal, lightWorldDir));

	float shadowFactor	= GetShadowFactor(data.shadowPos, shadowDepth, data.vertexWorldNormal, lightWorldDir);
	
	FragColor			= (FragColor * vec4(ambientColor, 1) ) + (FragColor * vec4(nDotL * lightColor, 1) * shadowFactor );
} 