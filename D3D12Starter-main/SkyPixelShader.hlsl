#include "GlobalShaderStructs.hlsli"

cbuffer SkyData : register(b1)
{
    uint cubemapIndex;
}


struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : TEXCOORD;
};

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    TextureCube skyTexture = ResourceDescriptorHeap[cubemapIndex];
    return skyTexture.Sample(BasicSampler, input.sampleDir);
}