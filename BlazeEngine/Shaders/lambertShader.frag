#version 430 core
#include "BlazeCommon.glsl"

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


// NOTE: For now, this struct must be exactly the same as the one in the frag shader. 
// TO DO: Implement shader #includes...
in struct VtoF
{
	vec3 vertexColor;
	vec3 fragNormal;
	vec2 uv0;
} data;

out vec4 FragColor;

void main()
{	
	FragColor = texture(albedo, data.uv0);

	float nDotL = max(0, dot(data.fragNormal, keyDirection));

	FragColor = (FragColor * vec4(ambient, 1) ) + (FragColor * vec4(nDotL * keyColor, 1));

//	FragColor = texture(normal, uv0);
//	FragColor = texture(roughness, uv0);
//	FragColor = texture(metallic, uv0);
//	FragColor = texture(ambientOcclusion, uv0);

} 