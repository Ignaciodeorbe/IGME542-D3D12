#include "GlobalShaderStructs.hlsli"

cbuffer SkyData : register(b0)
{
    matrix view;
    matrix projection;
}

struct VertexShaderInput
{
    float3 localPosition : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : TEXCOORD;
};

VertexToPixel_Sky main(VertexShaderInput input)
{
    VertexToPixel_Sky output;

    matrix viewNoTranslation = view;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;

    matrix projView = mul(projection, viewNoTranslation);
    output.position = mul(projView, float4(input.localPosition, 1.0f));
    output.position.z = output.position.w;
    output.sampleDir = input.localPosition;
    return output;
}