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
	// DEBUG: Just output an element
	FragColor = texture(GBuffer_Albedo, data.uv0.xy);
//	FragColor = texture(GBuffer_Normal, data.uv0.xy);
//	FragColor = texture(GBuffer_RMAO, data.uv0.xy);
//	FragColor = texture(GBuffer_Emissive, data.uv0.xy);
//	FragColor = texture(GBuffer_Position, data.uv0.xy);
} 