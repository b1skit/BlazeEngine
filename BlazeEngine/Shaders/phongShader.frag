#version 430 core
#define BLAZE_FRAGMENT_SHADER

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
	vec4 diffuseContribution	= vec4( LambertianDiffuse(FragColor.xyz, worldNormal, lightColor, lightWorldDir) , 1);

	// Specular:	
	vec4 specContribution		= vec4( PhongSpecular(data.worldPos, worldNormal, lightWorldDir, lightColor, in_view, RMAO.r, matProperty0.x) * FragColor.xyz, 1);

	// Shadows:
	float shadowFactor		= GetShadowFactor(data.shadowPos, shadowDepth, worldNormal, lightWorldDir);
	
	// Final result:
	FragColor = ambientContribution + ((diffuseContribution + specContribution) * shadowFactor);
} 