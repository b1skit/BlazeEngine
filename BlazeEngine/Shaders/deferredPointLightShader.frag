#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"
#include "BlazeLighting.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Window space fragment location. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

in vec4 gl_FragCoord;

void main()
{	
	vec2 uvs		= vec2(gl_FragCoord.x / screenParams.x, gl_FragCoord.y / screenParams.y); // [0, xRes/yRes] -> [0,1]

	// Cull based on depth:
	if (gl_FragCoord.z < texture( GBuffer_Depth, uvs).r)
	{
		discard;
	}
	
	// Sample textures once inside the main shader flow, and pass the values as required:
	FragColor					= texture(GBuffer_Albedo, uvs);

	vec3 worldNormal			= texture(GBuffer_WorldNormal, uvs).xyz;
	vec4 RMAO					= texture(GBuffer_RMAO, uvs);
	vec4 worldPosition			= texture(GBuffer_WorldPos, uvs);
	vec4 matProp0				= texture(GBuffer_MatProp0, uvs); // .r = Phong exponent

	vec3 fragToLight			= normalize(lightWorldPos - worldPosition.xyz);
	
	// Diffuse:
	vec4 diffuseContribution	= vec4( LambertianDiffuse(FragColor.xyz, worldNormal, lightColor, fragToLight) , 1);

	// Specular:
	vec4 specContribution		= vec4( PhongSpecular(worldPosition.xyz, worldNormal, fragToLight, lightColor, in_view, RMAO.r, matProp0.r).xyz * FragColor.xyz, 1);
	
	// Shadows:
	vec3 lightToFrag			= worldPosition.xyz - lightWorldPos; // Cubemap sampler direction length matters, so we can't use -fragToLight
	float shadowFactor			= GetShadowFactor(lightToFrag, CubeMap_0_Right, worldNormal, fragToLight);

	// Final result:
	FragColor = ((diffuseContribution + specContribution)) * LightAttenuation(worldPosition.xyz, lightWorldPos) * shadowFactor;	
} 