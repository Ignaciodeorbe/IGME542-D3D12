#include "GlobalShaderStructs.hlsli"

ConstantBuffer<SkyPixelShaderData> psData : register(b1);

SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    TextureCube skyTexture = ResourceDescriptorHeap[psData.cubemapIndex];
	return SkyboxTexture.Sample(BasicSampler, input.sampleDir);
}