// Blaze Engine GBuffer Shader. Fills GBuffer textures

#version 430 core

#define BLAZE_VERTEX_SHADER

#include "BlazeCommon.glsl"




//layout (location = 0) in vec4 gBuffer_in_albedo;
//layout (location = 1) in vec4 gBuffer_in_worldNormal;
//layout (location = 2) in vec4 gBuffer_in_RMAO; // I can use my existing samplers?????????
//layout (location = 3) in vec4 gBuffer_in_emissive;
////layout (location = 4) in vec4 gBuffer_in_depth;
//layout (location = 4) in vec4 gBuffer_in_position;


//out vec4 gBuffer_albedo;
//out vec4 gBuffer_worldNormal;
//out vec4 gBuffer_RMAO;
//out vec4 gBuffer_emissive;
//out vec4 gBuffer_depth;
//out vec4 gBuffer_position;


void main()
{
	// Assign position to the predefined gl_Position clip-space output:
    gl_Position				= in_mvp * vec4(in_position.x, in_position.y, in_position.z, 1.0);

	data.vertexColor		= in_color * vec4(ambient, 1);

	data.vertexWorldNormal	= (in_model * vec4(in_normal, 0.0f)).xyz;	// Normal -> World normal
	
	data.worldPos			= (in_model * vec4(in_position.xyz, 1.0f)).xyz;
//	data.shadowPos			= (shadowCam_vp * vec4(data.worldPos, 1)).xyz;

	data.uv0 = in_uv0;

//	gBuffer_albedo = vec4(0,1,0,1);
//	gBuffer_worldNormal = vec4(0,1,0,1);
}

// TODO: Review this, and make sure it's optimized!!!!!!!!