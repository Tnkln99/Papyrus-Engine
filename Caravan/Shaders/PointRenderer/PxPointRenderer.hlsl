#include "CmPointRenderer.hlsli"

float4 main(PSInput input) : SV_TARGET
{
    // Compute the distance from the center of the point sprite
    float distanceSquared = dot(input.localOffset, input.localOffset);

    // Discard the fragment if it lies outside the unit circle
    if (distanceSquared >= 1.0)
    {
        discard;
    }

    return float4(1.0f, 1.0f, 1.0f, 1.0);
}