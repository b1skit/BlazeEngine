#version 430 core

// Built-in input variables:
// in vec4 gl_FragCoord; //  location of the fragment in window space. 
// in bool gl_FrontFacing;
// in vec2 gl_PointCoord;

in vec4 vertexColor;
in vec3 fragNormal;
in vec2 uv0;

uniform vec4 ambient;

uniform vec3 keyDirection;
uniform vec4 keyColor;
uniform float keyIntensity;

uniform mat4 in_model;
uniform mat4 in_view;
uniform mat4 in_projection;
uniform mat4 in_mv;
uniform mat4 in_mvp;

layout (location = 0) uniform sampler2D albedo;
uniform sampler2D normal;
uniform sampler2D roughness;
uniform sampler2D metallic;
uniform sampler2D ambientOcclusion;

out vec4 FragColor;

void main()
{	
//	FragColor = vertexColor;

//	FragColor = texelFetch(albedo, ivec2(uv0.xy), 0);
//	FragColor = texture(albedo, uv0);
	FragColor = texture2D(albedo, uv0);
	
	
//	FragColor = vec4(uv0, 0,1); // Confirmed: UV's work!

	// Do we need to specify locations for frag shader? 
	// How do we link outputs from vert to inputs in frag? -> Using similar names!
} 