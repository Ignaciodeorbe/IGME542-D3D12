#include "GlobalShaderStructs.hlsli"

ConstantBuffer<SkyVertexShaderData> vsData : register(b0);

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
	// Set up output struct
	VertexToPixel_Sky output;

	// Copy of view matrix amd translations set to 0
    matrix viewNoTranslation = vsData.view;
	viewNoTranslation._14 = 0;
	viewNoTranslation._24 = 0;
	viewNoTranslation._34 = 0;

	// Multiplying projection and view matrix
    matrix projView = mul(vsData.projection, viewNoTranslation);

	// Apply projection and updated view matrix
	output.position = mul(projView, float4(input.localPosition, 1.0f));

	// change depth of each vertex 1.0
	output.position.z = output.position.w;

	// Compute sample direction 
	output.sampleDir = input.localPosition;


	return output;
}