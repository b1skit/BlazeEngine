#version 430 core
#define BLAZE_FRAGMENT_SHADER
#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
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
	float nDotL					= max(0, dot(texNormal, key_direction));
	vec4 diffuseContribution	= FragColor * vec4(nDotL * key_color, 1);

	// Specular:
	vec3 specColor	= key_color * texture(RMAO, data.uv0.xy).r;
	vec3 reflectDir = normalize(reflect(-key_direction, texNormal));		// World-space reflection dir
	reflectDir		= normalize((in_view * vec4(reflectDir, 0))).xyz;		// World -> view space
	
	vec3 viewDir	= normalize(data.viewPos.xyz);							// view-space fragment view dir
	
	float vDotR		= max(0, dot(viewDir, reflectDir));

	vec4 specContribution = vec4(specColor, 1) * pow(vDotR, matProperty0.x);

	// Final result:
	float shadowFactor = GetShadowFactor(data.worldPos, key_vp, shadowDepth, texNormal, key_direction); // TEMP: Pass key direction directly... Should be passing generic light's dir
	FragColor = ambientContribution + ((diffuseContribution + specContribution) * shadowFactor);
} 