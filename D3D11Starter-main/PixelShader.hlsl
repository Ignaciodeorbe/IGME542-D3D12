
#include "GlobalShaderStructs.hlsli"


// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;
    float2 uv				: TEXCOORD;
    float3 normal			: NORMAL;
    float3 tangent			: TANGENT;
    float3 worldPosition    : POSITION;
};

cbuffer PixelData : register(b0)
{
    uint albedoIndex;
    uint normalIndex;
    uint roughnessIndex;
    uint metalnessIndex;

    float2 uvScale;
    float2 uvOffset;

	float3 cameraPosition;

	int lightCount;
	Light lights[6];

};

SamplerState BasicSampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Adjusting scale and uv offset
    input.uv = input.uv * uvScale + uvOffset;
    
    Texture2D AlbedoTexture = ResourceDescriptorHeap[albedoIndex];
    Texture2D NormalMap = ResourceDescriptorHeap[normalIndex];
    Texture2D RoughnessMap = ResourceDescriptorHeap[roughnessIndex];
    Texture2D MetalnessMap = ResourceDescriptorHeap[metalnessIndex];
    
    // Sampling from textures
    float3 surfaceColor = pow(AlbedoTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
	
	// Assume albedo texture is actually holding specular color where metalness == 1
	// Note the use of lerp here - metal is generally 0 or 1, but might be in between
	// because of linear texture sampling, so we lerp the specular color to match
	float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);

	// Normal mapping
    input.normal = ComputeNormalMap(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);
    
    
	// Normalize normals
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
	
	// The variable for all the lighting
	float3 sceneLighting = surfaceColor * colorTint.rgb;

	float3 totalLight = float3(0, 0, 0);

	for (int i = 0; i < lightsCount; i++)
	{
		float3 lightResult = ComputeLighting(
			lights[i],
			input.normal,
			surfaceColor,
			cameraPosition,
			input.worldPosition,
			roughness,
			specularColor,
			metalness
		);


		totalLight += lightResult;

	}



	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	return float4(pow(sceneLighting, 1.0f / 2.2f), 1.0f);
}