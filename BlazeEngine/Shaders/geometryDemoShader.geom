#version 430 core

#define BLAZE_GEOMETRY_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

layout(location = 9) in VtoF inData[];
layout(location = 9) out VtoF data;

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


void main()
{
	for (int i = 0; i < 3; i++)
	{
		data = inData[i];

		// Quick-n-dirty normal offset, to test/demo geometry shader functionality:
		float offsetDistance = 0.2;
		gl_Position = gl_in[i].gl_Position + (in_projection * in_view * vec4(inData[i].vertexWorldNormal, 0) * offsetDistance);

        EmitVertex();
	}
    EndPrimitive();
} 
