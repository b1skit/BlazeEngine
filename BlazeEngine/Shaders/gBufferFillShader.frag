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
	gBuffer_out_albedo		= texture(albedo, data.uv0.xy);

	gBuffer_out_worldNormal = 
	(
		vec4
		(
			WorldNormalFromTexture
			(
				data.TBN, 
				texture(normal, data.uv0.xy).rgb, 
				in_modelRotation
			).xyz, 
			0
		)
	); // Could pack something else in .a??
	// ^^^ Issue: Doesn't work if the mesh doesn't have a normal? (Doesn't look right in RenderDoc)

	gBuffer_out_RMAO		= texture(RMAO, data.uv0.xy);
	gBuffer_out_emissive	= vec4(1,1,0,1); // Yellow debug for now...

	gBuffer_out_position	= vec4(data.worldPos.xyz, 0); // .a unused?
	// ^^Issue: Only records values in [0,1]...

	 gl_FragDepth			= gl_FragCoord.z; // Not actually needed, but this is what's happening
}