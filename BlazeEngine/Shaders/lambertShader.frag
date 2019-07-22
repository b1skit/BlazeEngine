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

	#if defined(NO_NORMAL_TEXTURE)
		float nDotL			= max(0, dot(data.vertexWorldNormal, lightWorldDir));
	#else
		vec3 texNormal	= ObjectNormalFromTexture(data.TBN, texture(normal, data.uv0.xy).rgb);
		texNormal		= (in_modelRotation * vec4(texNormal, 0)).xyz;		// Object -> world space
		// TODO: Use the function that gets the world normal right away??????

		float nDotL					= max(0, dot(texNormal, lightWorldDir));
	#endif

	float shadowFactor	= GetShadowFactor(data.shadowPos, shadowDepth, data.vertexWorldNormal, lightWorldDir);
	
	FragColor			= (FragColor * vec4(ambientColor, 1) ) + (FragColor * vec4(nDotL * lightColor, 1) * shadowFactor );
} 