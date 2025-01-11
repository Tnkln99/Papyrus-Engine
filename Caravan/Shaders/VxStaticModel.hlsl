#include "CmHelloTriangle.hlsli"

cbuffer SceneConstantBuffer : register(b0)
{
    float4 offset;
    float4 padding[15];
};

PSInput main(float4 position : POSITION, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = position + offset;
    result.uv = uv;

    return result;
}