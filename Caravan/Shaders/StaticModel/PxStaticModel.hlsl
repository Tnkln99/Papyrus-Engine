#include "CmStaticModel.hlsli"

float4 main(PSInput input) : SV_TARGET
{
    // Hard-coded directional light and colors
    float3 lightDirection = normalize(float3(0.0f, -1.0f, -1.0f));
    float3 lightColor     = float3(1.0f, 1.0f, 1.0f);
    float3 ambientColor   = float3(0.2f, 0.2f, 0.2f);

    // Normalize the normal coming from the vertex shader
    float3 N = normalize(input.normal.xyz);

    // Compute the Lambertian diffuse factor
    float diffuse = saturate(dot(N, lightDirection));

    // Combine ambient and diffuse contributions
    float3 color = ambientColor + (diffuse * lightColor);

    // Return final color with alpha = 1.0
    return float4(color, 1.0f);
}