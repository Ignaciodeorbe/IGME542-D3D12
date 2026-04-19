#include "Skybox.h"
#include "Graphics.h"

using Microsoft::WRL::ComPtr;

Skybox::Skybox(
    const wchar_t* right,
    const wchar_t* left,
    const wchar_t* up,
    const wchar_t* down,
    const wchar_t* front,
    const wchar_t* back,
    std::shared_ptr<Mesh> cubeMesh,
    ComPtr<ID3DBlob> vertexShaderBytecode,
    ComPtr<ID3DBlob> pixelShaderBytecode)
    : cubeMesh(cubeMesh)
{
    // Load all 6 faces into a cubemap and store the bindless SRV index
    cubemapSRVIndex = Graphics::CreateCubemap(right, left, up, down, front, back);

    CreateSkyboxPSO(vertexShaderBytecode, pixelShaderBytecode);
}

Skybox::~Skybox() {}

// binds sky PSO and draws the cube
void Skybox::Draw(std::shared_ptr<Camera> camera)
{
    auto commandList = Graphics::CommandList;

    // Set sky PSO and root signature
    commandList->SetPipelineState(pipelineState.Get());
    commandList->SetGraphicsRootSignature(rootSignature.Get());

    // Bind the main SRV/CBV descriptor heap
    ID3D12DescriptorHeap* heaps[] = { Graphics::CBVSRVDescriptorHeap.Get() };
    commandList->SetDescriptorHeaps(1, heaps);


    DirectX::XMFLOAT4X4 view = camera->ViewMatrix();
    DirectX::XMFLOAT4X4 proj = camera->ProjectionMatrix();

    commandList->SetGraphicsRoot32BitConstants(0, 16, &view, 0);
    commandList->SetGraphicsRoot32BitConstants(0, 16, &proj, 16);

    commandList->SetGraphicsRoot32BitConstants(1, 1, &cubemapSRVIndex, 0);

    // Bind vertex and index buffers and draw
    D3D12_VERTEX_BUFFER_VIEW vbView = cubeMesh->GetVertexBufferView();
    D3D12_INDEX_BUFFER_VIEW  ibView = cubeMesh->GetIndexBufferView();
    commandList->IASetVertexBuffers(0, 1, &vbView);
    commandList->IASetIndexBuffer(&ibView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawIndexedInstanced(cubeMesh->GetIndexCount(), 1, 0, 0, 0);
}

// Build a PSO for skybox rendering
void Skybox::CreateSkyboxPSO(
    ComPtr<ID3DBlob> vertexShaderBytecode,
    ComPtr<ID3DBlob> pixelShaderBytecode)
{
    
 
    D3D12_ROOT_PARAMETER params[2] = {};

    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    params[0].Constants.ShaderRegister = 0; // b0
    params[0].Constants.RegisterSpace = 0;
    params[0].Constants.Num32BitValues = 32;

    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    params[1].Constants.ShaderRegister = 1; 
    params[1].Constants.RegisterSpace = 0;
    params[1].Constants.Num32BitValues = 1;

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0; 
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
    rsDesc.NumParameters = 2;
    rsDesc.pParameters = params;
    rsDesc.NumStaticSamplers = 1;
    rsDesc.pStaticSamplers = &sampler;
    rsDesc.Flags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

    ComPtr<ID3DBlob> rsBlob, errorBlob;
    D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rsBlob, &errorBlob);
    Graphics::Device->CreateRootSignature(
        0, rsBlob->GetBufferPointer(), rsBlob->GetBufferSize(),
        IID_PPV_ARGS(rootSignature.GetAddressOf()));
 

    D3D12_INPUT_ELEMENT_DESC inputElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    D3D12_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D12_FILL_MODE_SOLID;
    rastDesc.CullMode = D3D12_CULL_MODE_FRONT; 
    rastDesc.FrontCounterClockwise = false;
    rastDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rastDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    rastDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    rastDesc.DepthClipEnable = true;
    rastDesc.MultisampleEnable = false;
    rastDesc.AntialiasedLineEnable = false;
    rastDesc.ForcedSampleCount = 0;
    rastDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = true;
    depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    depthDesc.StencilEnable = false;

    D3D12_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = false;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();
    psoDesc.VS = { vertexShaderBytecode->GetBufferPointer(),
                                      vertexShaderBytecode->GetBufferSize() };
    psoDesc.PS = { pixelShaderBytecode->GetBufferPointer(),
                                      pixelShaderBytecode->GetBufferSize() };
    psoDesc.InputLayout = { inputElements, _countof(inputElements) };
    psoDesc.RasterizerState = rastDesc;
    psoDesc.DepthStencilState = depthDesc;
    psoDesc.BlendState = blendDesc;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; 
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    psoDesc.SampleDesc.Count = 1;

    Graphics::Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
}