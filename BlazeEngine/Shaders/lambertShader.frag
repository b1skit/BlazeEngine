#version 430 core
#include "BlazeCommon.glsl"
#include "BlazeFragmentCommon.glsl"

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


//out vec4 FragColor;

void main()
{	
	FragColor = texture(albedo, data.uv0.xy);

	float nDotL = max(0, dot(data.fragWorldNormal, keyDirection));

	FragColor = (FragColor * vec4(ambient, 1) ) + (FragColor * vec4(nDotL * keyColor, 1));

//	FragColor = texture(normal, uv0);
//	FragColor = texture(roughness, uv0);
//	FragColor = texture(metallic, uv0);
//	FragColor = texture(ambientOcclusion, uv0);

} 