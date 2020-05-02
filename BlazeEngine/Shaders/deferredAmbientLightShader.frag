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


#if defined AMBIENT_IBL

uniform int maxMipLevel;	// Largest mip level in the PMREM cube map texture (CubeMap_1). Uploaded during ImageBasedLight setup

void main()
{	
	// Cull based on depth: Don't bother lighting unless the fragment is in front of the far plane (prevents ambient lighting of the far plane...)
	if (texture( GBuffer_Depth, data.uv0.xy).r == 1.0)
	{
		discard;
	}
	// TEMP HACK!!!
	// TODO: Fix Ambient/Directional lights: Flip screen-aligned quad and render back faces (to be consistent with other deferred lights)


	FragColor				= texture(GBuffer_Albedo, data.uv0.xy);		// Note: For PBR, we require all calculations to be performed in linear color
	FragColor.rgb			= Degamma(FragColor.rgb);


	vec3 worldNormal		= texture(GBuffer_WorldNormal, data.uv0.xy).xyz;
	vec4 RMAO				= texture(GBuffer_RMAO, data.uv0.xy);
	vec4 worldPosition		= texture(GBuffer_WorldPos, data.uv0.xy);
	vec4 matProp0			= texture(GBuffer_MatProp0, data.uv0.xy);	// .rgb = F0 (Surface response at 0 degrees), .a = Phong exponent

	float AO				= RMAO.b;
	float metalness			= RMAO.y;

	vec4 viewPosition		= in_view * worldPosition;							// View-space position
	vec3 viewEyeDir			= normalize(-viewPosition.xyz);						// View-space eye/camera direction
	vec3 viewNormal			= normalize(in_view * vec4(worldNormal, 0)).xyz;	// View-space surface normal

	float NoV				= max(0.0, dot(viewNormal, viewEyeDir) );

	vec3 F0					= matProp0.rgb; // .rgb = F0 (Surface response at 0 degrees), .a = Phong exponent
	F0						= mix(F0, FragColor.rgb, metalness); // Linear interpolation: x, y, using t=[0,1]. Returns x when t=0 -> Blends towards albedo for metals

//	vec3 fresnel_kS			= FresnelSchlick(NoV, F0); // Doesn't quite look right: Use FresnelSchlick_Roughness() instead
	vec3 fresnel_kS			= FresnelSchlick_Roughness(NoV, F0, RMAO.x);
	vec3 k_d				= 1.0 - fresnel_kS;	

	// Sample the diffuse irradiance from our prefiltered irradiance environment map:
	vec3 irradiance			= texture(CubeMap_0, worldNormal).xyz;


	// Get the specular reflectance term:
	vec3 worldView			= normalize(cameraWorldPos - worldPosition.xyz);	// Direction = Point -> Eye
	vec3 worldReflection	= normalize(reflect(-worldView, worldNormal));

	vec2 BRDF				= texture(texture0, vec2(max(NoV, 0.0), RMAO.x) ).rg;	// Sample our generated BRDF Integration map
	vec3 specular			= textureLod(CubeMap_1, worldReflection, RMAO.x * maxMipLevel).xyz * ((fresnel_kS * BRDF.x) + BRDF.y);



	FragColor				= vec4((FragColor.rgb * irradiance * k_d + specular) * AO, 1.0); // Note: Omitted the "/ PI" factor here
//	FragColor				= vec4((FragColor.rgb * irradiance * k_d + specular) * AO / PI, 1.0); // Note: Omitted the "/ PI" factor here
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