#version 430 core

#define BLAZE_FRAGMENT_SHADER
#define BLAZE_VEC4_OUTPUT

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"
#include "BlazeLighting.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


void main()
{	
	FragColor			= texture(albedo, data.uv0.xy);
	
	vec3 worldNormal	= WorldNormalFromTexture(normal, data.uv0.xy, data.TBN);

	vec4 RMAO			= texture(RMAO, data.uv0.xy);

	// Ambient:
	vec4 ambientContribution	= FragColor * vec4(ambientColor, 1);

	// Diffuse:
	vec4 diffuseContribution	= vec4( LambertianDiffuse(FragColor.xyz, worldNormal, lightColor, keylightWorldDir) , 1);

	// Specular:	
	vec4 specContribution		= vec4( PhongSpecular(data.worldPos, worldNormal, keylightWorldDir, lightColor, in_view, RMAO.r, matProperty0.a) * FragColor.xyz, 1);

	// NOTE: keylightWorldDir is Key light's -forward direction (This shader only supports the keylight)

	// Shadows:
	float NoL					= max(0.0, dot(worldNormal, keylightWorldDir));
	float shadowFactor			= GetShadowFactor(data.shadowPos, shadowDepth, NoL);
	
	// Final result:
	FragColor = ambientContribution + ((diffuseContribution + specContribution) * shadowFactor);
} 