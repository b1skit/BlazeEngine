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

	

	vec3 texNormal	= ObjectNormalFromTexture(data.TBN, texture(normal, data.uv0.xy).rgb);
	texNormal		= (in_modelRotation * vec4(texNormal, 0)).xyz;		// Object -> world space

	// Ambient:
	vec4 ambientContribution	= FragColor * vec4(ambient, 1);

	// Diffuse:
	float nDotL					= max(0, dot(texNormal, keyDirection));
	vec4 diffuseContribution	= FragColor * vec4(nDotL * keyColor, 1);

	// Specular:
	vec3 specColor	= keyColor * texture(RMAO, data.uv0.xy).r;
	vec3 reflectDir = normalize(reflect(-keyDirection, texNormal));		// World-space reflection dir
	reflectDir		= normalize((in_view * vec4(reflectDir, 0))).xyz;		// World -> view space
	
	vec3 viewDir	= normalize(data.viewPos.xyz);							// view-space fragment view dir
	
	float vDotR		= max(0, dot(viewDir, reflectDir));

	vec4 specContribution = vec4(specColor, 1) * pow(vDotR, matProperty0.x);

	// Final result:
	FragColor = ambientContribution + diffuseContribution + specContribution;



	// DEBUG:!!!!!
	//Depth-component textures are treated as one-component floating-point textures:
	float depthVal = texture(key_depth, data.uv0.xy).r; // Depth buffer contains (depth, 0, 0, 0)

	FragColor = vec4(depthVal, depthVal, depthVal, 1);	
} 