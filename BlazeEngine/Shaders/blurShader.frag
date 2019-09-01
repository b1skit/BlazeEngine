#version 430 core

#define BLAZE_FRAGMENT_SHADER

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"

// Built-in input variables:
//layout(pixel_center_integer) in vec4 gl_FragCoord; //  Location of the fragment in window space. (x,y,z,w) = window-relative (x,y,z,1/w)
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

#define RGB_TO_LUMINANCE vec3(0.2126, 0.7152, 0.0722)
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

#endif

// Pass 0: Blur luminance threshold:
#if defined(BLUR_SHADER_LUMINANCE_THRESHOLD)

	void main()
	{	
		vec4 fragRGB	= texture(GBuffer_Albedo, data.uv0.xy);

		float luminance	= dot(fragRGB.rgb, RGB_TO_LUMINANCE);

		FragColor = luminance > 1.0 ? fragRGB : vec4(0.0, 0.0, 0.0, 1.0);
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
		uvs.y += texelSize.y * TEXEL_OFFSET;	// Offset up by 2 pixels

		for (int i = 0; i < NUM_TAPS; i++)
		{
			total += texture(GBuffer_Albedo, uvs).rgb * weights[i];

			uvs.y -= texelSize.y; // Move the sample right by 1 pixel
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
