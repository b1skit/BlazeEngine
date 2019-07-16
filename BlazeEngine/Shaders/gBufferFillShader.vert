// Blaze Engine GBuffer Shader. Fills GBuffer textures

#version 430 core

#define BLAZE_VERTEX_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

void main()
{
	// Assign position to the predefined gl_Position clip-space output:
    gl_Position				= in_mvp * vec4(in_position.x, in_position.y, in_position.z, 1.0);
	
	data.worldPos			= (in_model * vec4(in_position.xyz, 1.0f)).xyz;

	data.uv0				= in_uv0;

	data.TBN				= AssembleTBN(in_tangent, in_bitangent, in_modelRotation);
}