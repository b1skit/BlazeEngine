#version 430 core

#define BLAZE_FRAGMENT_SHADER
#define BLAZE_VEC4_OUTPUT

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;


#if !defined(BLUR_SHADER_LUMINANCE_THRESHOLD)
	
	

	// 5-tap Gaussian filter:
//	#define NUM_TAPS 5
//	#define TEXEL_OFFSET 2
//	uniform float weights[NUM_TAPS] = float[] (0.06136, 0.24477, 0.38774, 0.24477, 0.06136);		// 5 tap filter
//	uniform float weights[NUM_TAPS] = float[] (0.060626, 0.241843, 0.383103, 0.241843, 0.060626);	// Note: This is a 7 tap filter, but we ignore the outer 2 samples as they're only 0.00598

	// 9-tap Gaussian filter:
	#define NUM_TAPS 7
	#define TEXEL_OFFSET 3
	uniform float weights[NUM_TAPS] = float[] (0.005977, 0.060598, 0.241732, 0.382928, 0.241732, 0.060598, 0.005977);	// Note: This is a 9 tap filter, but we ignore the outer 2 samples as they're only 0.000229

//	// 11-tap Gaussian filter:
//	#define NUM_TAPS 9
//	#define TEXEL_OFFSET 4
//	uniform float weights[NUM_TAPS] = float[] (	0.000229, 0.005977, 0.060598, 0.24173, 0.382925, 0.24173, 0.060598, 0.005977, 0.000229);	// Note: This is a 11-tap filter, but we ignore the outer 2 samples as they're only 0.000003



#endif

// Pass 0: Blur luminance threshold:
#if defined(BLUR_SHADER_LUMINANCE_THRESHOLD)

	#define RAMP_POWER 2.0
	#define SPEED 0.05

	void main()
	{	
		// Sigmoid function tuning: https://www.desmos.com/calculator/w3hrskwpyb

		vec3 fragRGB	= texture(GBuffer_Albedo, data.uv0.xy).rgb;
		
		float maxChannel = max(fragRGB.x, max(fragRGB.y, fragRGB.z));
		float scale		= pow(SPEED * maxChannel, RAMP_POWER);
		scale			= scale / (scale + 1.0);

		FragColor = vec4(fragRGB * scale, 1.0);
	}


// Pass 1: Horizontal blur:
#elif defined(BLUR_SHADER_HORIZONTAL)

	void main()
	{
		vec3 total = vec3(0,0,0);
		vec2 uvs = data.uv0.xy;
		uvs.x -= texelSize.x * TEXEL_OFFSET;	// Offset to the left

		for (int i = 0; i < NUM_TAPS; i++)
		{
			total += texture(GBuffer_Albedo, uvs).rgb * weights[i];

			uvs.x += texelSize.x; // Move the sample right by 1 pixel
		}

		FragColor = vec4(total, 1);
	} 


// Pass 2: Vertical blur:
#elif defined(BLUR_SHADER_VERTICAL)
	
	void main()
	{	
		vec3 total = vec3(0,0,0);
		vec2 uvs = data.uv0.xy;
		uvs.y += texelSize.y * TEXEL_OFFSET;	// Offset

		for (int i = 0; i < NUM_TAPS; i++)
		{
			total += texture(GBuffer_Albedo, uvs).rgb * weights[i];

			uvs.y -= texelSize.y; // Move the sample down by 1 pixel
		}

		FragColor = vec4(total, 1);
	} 


// Error:
#else

	void main()
	{	
		FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f); // Hot pink
	} 

#endif
