#include "CmPointRenderer.hlsli"

cbuffer Camera : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float4 padding[8];
};

struct InstanceData
{
    float3 position;
    float radius;
};

// Define triangle offsets for a point sprite in NDC space
static const float2 offset[6] = {
    float2(-1.0, -1.0),
    float2(-1.0, 1.0),
    float2(1.0, -1.0),
    float2(1.0, -1.0),
    float2(-1.0, 1.0),
    float2(1.0, 1.0)
};

// Input structured buffer of instance data
StructuredBuffer<InstanceData> instanceDataBuffer : register(t0);

struct VSInput
{
    uint vertexID : SV_VertexID;
    uint instanceID : SV_InstanceID;
};

PSInput main(VSInput input)
{
    PSInput result;

    // Get the instance data for the current instance
    InstanceData instance = instanceDataBuffer[input.instanceID];

    // Compute the world position of the point instance
    float4 worldPos = float4(instance.position, 1.0);

    // Transform to view space
    float4 viewPos = mul(worldPos, view);

    // Transform to clip space
    float4 clipPos = mul(viewPos, projection);

    // Add per-vertex offset to create a quad (point sprite)
    float2 localOffset = offset[input.vertexID];
	float perspectiveRadius = instance.radius / clipPos.w;
    clipPos.xy += localOffset * perspectiveRadius; // Scale by radius and perspective

    // Output final position and local offset
    result.position = clipPos;
    result.localOffset = localOffset;

    return result;
}
