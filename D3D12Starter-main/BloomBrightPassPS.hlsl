Texture2D sceneTexture : register(t0);
SamplerState basicSampler : register(s0);

struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float4 color = sceneTexture.Sample(basicSampler, input.uv);
    float luminance = dot(color.rgb, float3(0.2, 0.7, 0.07));
    if (luminance < 0.6f)
        return float4(0, 0, 0, 1);
    return color;
}