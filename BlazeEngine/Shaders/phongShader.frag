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
	FragColor		= texture(albedo, data.uv0.xy);
	float specColor = texture(RMAO, data.uv0.xy).r;

	vec3 texNormal	= ObjectNormalFromTexture(data.TBN, texture(normal, data.uv0.xy).rgb);
	texNormal		= (in_modelRotation * vec4(texNormal, 0)).xyz;		// Object -> world space

	// Ambient:
	vec4 ambientContribution = FragColor * vec4(ambient, 1);

	// Diffuse:
	float nDotL = max(0, dot(texNormal, keyDirection));
	vec4 diffuseContribution = FragColor * vec4(nDotL * keyColor, 1);

	// Specular:	
	vec3 reflectDir = normalize(reflect(-keyDirection, texNormal));		// World-space reflection dir
	reflectDir = normalize((in_view * vec4(reflectDir, 0))).xyz;		// World -> view space

	vec3 viewDir = normalize(data.viewPos.xyz);							// view-space fragment view dir

	float vDotR = max(0, dot(viewDir, reflectDir));

	float specChannelVal = specColor * pow(vDotR, matProperty0.x);
	vec4 specContribution = vec4(specChannelVal, specChannelVal, specChannelVal, 1); //DEBUG

	// Final result:
	FragColor = ambientContribution + diffuseContribution + specContribution;
} 