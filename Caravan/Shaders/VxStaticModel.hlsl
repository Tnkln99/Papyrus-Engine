#include "CmStaticModel.hlsli"

cbuffer Camera : register(b0)
{
	float4x4 view;
    float4x4 projection;
	float4 padding[8];
};

struct InstanceData
{
    float4x4 modelMatrix;
};

StructuredBuffer<InstanceData> instanceDataBuffer : register(t0);


PSInput main(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
   	PSInput result;

    // Fetch instance model matrix
    float4x4 modelMatrix = instanceDataBuffer[instanceID].modelMatrix;

	// World space
    float4 worldPos = mul(position, modelMatrix);
    // Camera (view) space
    float4 viewPos  = mul(worldPos, view);
    // Clip space
    float4 clipPos  = mul(viewPos, projection);

	result.position = clipPos;

	// Transform normal (using the 3x3 part of the modelMatrix)
    // If non-uniform scaling is present, use inverse-transpose of modelMatrix’s 3x3 for correct lighting.
    float3 worldNormal = mul(modelMatrix, normal);
    // Pass it as float4 but only the xyz is relevant for shading
    result.normal = float4(worldNormal, 0.0f);

    result.uv = uv;

    return result;
}