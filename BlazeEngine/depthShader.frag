// Blaze Engine Depth Shader

#version 430 core

void main()
{
    gl_FragDepth = gl_FragCoord.z; // Not actually needed, but this is what's happening
} 