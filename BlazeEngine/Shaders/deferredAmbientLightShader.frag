#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"
#include "BlazeLighting.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


#if defined AMBIENT_IBL

void main()
{	
	// Cull based on depth: Don't bother lighting unless the fragment is in front of the far plane (prevents ambient lighting of the far plane...)
	if (texture( GBuffer_Depth, data.uv0.xy).r == 1.0)
	{
		discard;
	}
	// TEMP HACK!!!
	// TODO: Fix Ambient/Directional lights: Flip screen-aligned quad and render back faces (to be consistent with other deferred lights)


	// TODO: Merge duplicated code with BlazeLighting::ComputePBRLighting() ?
	// -> Add the diffuse irradiance contribution to every fragment, instead of drawing it in a separate pass/draw call (then we'd only need a diffuse light in forward mode?)

	FragColor			= texture(GBuffer_Albedo, data.uv0.xy);		// Note: For PBR, we require all calculations to be performed in linear color
	FragColor.rgb		= Degamma(FragColor.rgb);


	vec3 worldNormal	= texture(GBuffer_WorldNormal, data.uv0.xy).xyz;
	vec4 RMAO			= texture(GBuffer_RMAO, data.uv0.xy);
	vec4 worldPosition	= texture(GBuffer_WorldPos, data.uv0.xy);
	vec4 matProp0		= texture(GBuffer_MatProp0, data.uv0.xy); // .rgb = F0 (Surface response at 0 degrees), .a = Phong exponent

	float AO			= RMAO.b;
	float metalness		= RMAO.y;

	vec4 viewPosition	= in_view * worldPosition;							// View-space position
	vec3 viewEyeDir		= normalize(-viewPosition.xyz);						// View-space eye/camera direction
	vec3 viewNormal		= normalize(in_view * vec4(worldNormal, 0)).xyz;	// View-space surface normal

	float NoV			= max(0.0, dot(viewNormal, viewEyeDir) );

	vec3 F0				= matProp0.rgb; // .rgb = F0 (Surface response at 0 degrees), .a = Phong exponent
	F0					= mix(F0, FragColor.rgb, metalness); // Linear interpolation: x, y, using t=[0,1]. Returns x when t=0 -> Blends towards albedo for metals

	vec3 fresnel		= FresnelSchlick(NoV, F0);
	vec3 k_d			= 1.0 - fresnel;	

	// Sample the diffuse irradiance from our prefiltered irradiance environment map:
	vec3 irradiance		= texture(CubeMap_0_Right, worldNormal).xyz;

	FragColor			= vec4(FragColor.rgb * irradiance * AO * k_d, 1.0); // Note: Omitted the "/ PI" factor here
//	FragColor			= vec4(FragColor.rgb * irradiance * AO * k_d / PI, 1.0);

}


#else
// ie. AMBIENT_COLOR: No IBL found, fallback to using an ambient color


void main()
{	
	float AO = texture(GBuffer_RMAO, data.uv0.xy).b;

	// Phong ambient contribution:
	FragColor	= texture(GBuffer_Albedo, data.uv0.xy) * vec4(lightColor, 1) * AO;	
}

#endif