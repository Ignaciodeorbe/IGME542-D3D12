#include "GlobalShaderStructs.hlsli"


ConstantBuffer<SkyPixelShaderData> psData : register(b1);

SamplerState BasicSampler : register(s0);

struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : TEXCOORD;
};

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    TextureCube SkyboxTexture = ResourceDescriptorHeap[psData.cubemapIndex];
	return SkyboxTexture.Sample(BasicSampler, input.sampleDir);
}