// Blaze Engine Depth Shader

#version 430 core

uniform vec3 lightWorldPos;	

uniform float shadowCam_near;
uniform float shadowCam_far;

in vec4 FragPos; // Projection space

void main()
{
	float lightDistance = length(FragPos.xyz - lightWorldPos);
    
    // Map to [0, 1]:
	lightDistance = lightDistance / shadowCam_far; // TODO: Correct for perspective and write a linear depth
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
} 