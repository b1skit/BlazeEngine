#version 430 core

#define BLAZE_FRAGMENT_SHADER
//#define BLAZE_GBUFFER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


void main()
{	
//	FragColor		= texture(albedo, data.uv0.xy);
//
//	
//
//	vec3 texNormal	= ObjectNormalFromTexture(data.TBN, texture(normal, data.uv0.xy).rgb);
//	texNormal		= (in_modelRotation * vec4(texNormal, 0)).xyz;		// Object -> world space
//
//	// Ambient:
//	vec4 ambientContribution	= FragColor * vec4(ambient, 1);
//
//	// Diffuse:
//	float nDotL					= max(0, dot(texNormal, lightDirection));
//	vec4 diffuseContribution	= FragColor * vec4(nDotL * lightColor, 1);
//
//	// Specular:
//	vec3 specColor	= lightColor * texture(RMAO, data.uv0.xy).r;
//	vec3 reflectDir = normalize(reflect(-lightDirection, texNormal));		// World-space reflection dir
//	reflectDir		= normalize((in_view * vec4(reflectDir, 0))).xyz;		// World -> view space
//	
//	vec3 viewDir			= normalize(data.viewPos.xyz);					// view-space fragment view dir
//	float vDotR				= max(0, dot(viewDir, reflectDir));
//	vec4 specContribution	= vec4(specColor, 1) * pow(vDotR, matProperty0.x);
//
//	float shadowFactor		= GetShadowFactor(data.shadowPos, shadowDepth, data.vertexWorldNormal, lightDirection);
//	
//	// Final result:
//	FragColor = ambientContribution + ((diffuseContribution + specContribution) * shadowFactor);

//	FragColor = vec4(texNormal, 1);
	
//	FragColor = vec4(1,0,0,1);

//	FragColor = vec4(1,0,0,1) + texture(GBuffer_Albedo, data.uv0.xy);
	FragColor = texture(GBuffer_Albedo, data.uv0.xy);
//	FragColor = vec4(data.uv0.xy, 0,1);

//	FragColor = data.vertexColor; // UV's ARE working

//	FragColor = 
//	texture(GBuffer_Albedo, data.uv0.xy) + 
//	texture(GBuffer_Normal, data.uv0.xy) + 
//	texture(GBuffer_RMAO, data.uv0.xy) + 
//	texture(GBuffer_Emissive, data.uv0.xy) +
//	
//	texture(albedo, data.uv0.xy) +
//	texture(normal, data.uv0.xy) +
//	texture(RMAO, data.uv0.xy) +
//	texture(shadowDepth, data.uv0.xy);




} 