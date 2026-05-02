Texture2D sceneTexture : register(t0);
Texture2D bloomTexture : register(t1);
SamplerState basicSampler : register(s0);

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float4 scene = sceneTexture.Sample(basicSampler, input.uv);
    float4 bloom = bloomTexture.Sample(basicSampler, input.uv);
    return scene + bloom * 0.8f;
}