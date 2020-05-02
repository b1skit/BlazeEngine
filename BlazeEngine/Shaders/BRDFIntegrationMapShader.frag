#version 430 core

#define BLAZE_FRAGMENT_SHADER
#define BLAZE_VEC2_OUTPUT

#include "BlazeCommon.glsl"
#include "BlazeGlobals.glsl"
#include "BlazeLighting.glsl"

// Generate a BRDF Integration map lookup texture:
// Returns the BRDF Fresnel response, given the dot product of surface normal and incoming light direction, and the surface roughness
// UVs.x == NoV, UVs.y == roughness
// Return.r == BRDF Fresnel response scale
// Return.g == BRDF Fresnel response bias
vec2 UVsToIntegratedBRDF(vec2 UVs)
{
    float NoV       = UVs.x;
    float NoV2      = NoV * NoV;
    float roughness = UVs.y;

    vec3 V = vec3(sqrt(1.0 - NoV2), 0.0, NoV);

    float fresnelScale = 0.0;
    float fresnelBias = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const int NUM_SAMPLES = 1024;
    for(uint i = 0; i < NUM_SAMPLES; i++)
    {
        vec2 Xi = Hammersley2D(i, NUM_SAMPLES);
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NoL = max(L.z, 0.0);
        float NoH = max(H.z, 0.0);
        float VoH = max(dot(V, H), 0.0);

        if(NoL > 0.0)
        {
            float G     = GeometrySmith(NoV, NoL, roughness);
            float GVis = (G * VoH) / (NoH * NoV);
            float Fc    = pow(1.0 - VoH, 5.0);

            fresnelScale += (1.0 - Fc) * GVis;
            fresnelBias  += Fc * GVis;
        }
    }

    // Average the results:
    fresnelScale /= float(NUM_SAMPLES);
    fresnelBias /= float(NUM_SAMPLES);

    return vec2(fresnelScale, fresnelBias);
}


void main() 
{
    vec2 BRDFValue  = UVsToIntegratedBRDF(data.uv0.xy);

    FragColor       = vec2(BRDFValue.x, BRDFValue.y);
}