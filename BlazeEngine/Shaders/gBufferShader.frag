#version 430 core

#define BLAZE_FRAGMENT_SHADER
#define BLAZE_GBUFFER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;



//in vec4 gBuffer_albedo;
//in vec4 gBuffer_worldNormal;
//in vec4 gBuffer_RMAO;
//in vec4 gBuffer_emissive;
//in vec4 gBuffer_position;

//layout (location = 0) out vec4 gBuffer_out_albedo;
//layout (location = 1) out vec4 gBuffer_out_worldNormal;
//layout (location = 2) out vec4 gBuffer_out_RMAO;
//layout (location = 3) out vec4 gBuffer_out_emissive;
//layout (location = 4) out vec4 gBuffer_out_position;
//// gBuffer depth == gl_FragDepth

out vec4 gBuffer_out_albedo;
out vec4 gBuffer_out_worldNormal;
out vec4 gBuffer_out_RMAO;
out vec4 gBuffer_out_emissive;
out vec4 gBuffer_out_position;
// gBuffer depth == gl_FragDepth


void main()
{	
//	FragColor			= texture(albedo, data.uv0.xy);
//
//	float nDotL			= max(0, dot(data.vertexWorldNormal, lightDirection));
//
//	float shadowFactor	= GetShadowFactor(data.shadowPos, shadowDepth, data.vertexWorldNormal, lightDirection); // TEMP: Pass key direction directly... Should be passing generic light's dir
//	
//	FragColor			= (FragColor * vec4(ambient, 1) ) + (FragColor * vec4(nDotL * lightColor, 1) * shadowFactor );

//	FragColor = vec4(1,0,0,1);

//	gBuffer_out_albedo = vec4(1,0,0,1);
	gBuffer_out_albedo = texture(albedo, data.uv0.xy);
	gBuffer_out_worldNormal = vec4(1,0,0,1);
	gBuffer_out_RMAO = vec4(1,0,0,1);
	gBuffer_out_emissive = vec4(0,1,0,1);

	gBuffer_out_position = vec4(0,0,1,1);// DEPTH

	 gl_FragDepth = gl_FragCoord.z; // Not actually needed, but this is what's happening
}

// ^^ GARBAGE!! Just trying to get it to compile for debugging....