struct VertexOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOutput main(uint id : SV_VertexID)
{
    VertexOutput output;
    float2 uvs[3] =
    {
        float2(0, 0),
        float2(2, 0),
        float2(0, 2)
    };
    output.uv = uvs[id];
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}