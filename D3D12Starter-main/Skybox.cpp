#include "Skybox.h"
#include "WICTextureLoader.h"
#include "PathHelpers.h"


using namespace DirectX;


Skybox::Skybox(std::shared_ptr<Mesh> mesh, unsigned int cubemapIndex)
	: skyboxMesh(mesh),	cubemapIndex(cubemapIndex)
{
    InitRenderStates();

}

// Destructor
Skybox::~Skybox()
{
}

void Skybox::InitRenderStates()
{
    // ---------- ROOT SIGNATURE ----------
    D3D12_ROOT_PARAMETER rootParams[1] = {};

    rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
    rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    rootParams[0].Constants.Num32BitValues = sizeof(SkyDrawIndices) / sizeof(unsigned int);
    rootParams[0].Constants.RegisterSpace = 0;
    rootParams[0].Constants.ShaderRegister = 0;

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.Filter = D3D12_FILTER_ANISOTROPIC;
    sampler.MaxAnisotropy = 16;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    rootSigDesc.NumParameters = 1;
    rootSigDesc.pParameters = rootParams;
    rootSigDesc.NumStaticSamplers = 1;
    rootSigDesc.pStaticSamplers = &sampler;

    Microsoft::WRL::ComPtr<ID3DBlob> serialized;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;

    D3D12SerializeRootSignature(
        &rootSigDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        serialized.GetAddressOf(),
        errors.GetAddressOf());

    Graphics::Device->CreateRootSignature(
        0,
        serialized->GetBufferPointer(),
        serialized->GetBufferSize(),
        IID_PPV_ARGS(rootSignature.GetAddressOf()));

    // ---------- PSO ----------
    Microsoft::WRL::ComPtr<ID3DBlob> vs;
    Microsoft::WRL::ComPtr<ID3DBlob> ps;

    D3DReadFileToBlob(FixPath(L"SkyVS.cso").c_str(), vs.GetAddressOf());
    D3DReadFileToBlob(FixPath(L"SkyPS.cso").c_str(), ps.GetAddressOf());

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = rootSignature.Get();

    psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };

    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    psoDesc.SampleDesc.Count = 1;

    // CRITICAL SKY SETTINGS
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    psoDesc.RasterizerState.DepthClipEnable = true;

    psoDesc.DepthStencilState.DepthEnable = true;
    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

    psoDesc.SampleMask = 0xffffffff;

    Graphics::Device->CreateGraphicsPipelineState(
        &psoDesc,
        IID_PPV_ARGS(pipelineState.GetAddressOf()));
}

void Skybox::Draw(
    ID3D12GraphicsCommandList* commandList,
    std::shared_ptr<Camera> camera)
{
    commandList->SetPipelineState(pipelineState.Get());
    commandList->SetGraphicsRootSignature(rootSignature.Get());

    SkyDrawIndices drawData{};
    drawData.psSkyboxIndex = cubemapIndex;
    drawData.vsVertexBufferIndex =
        Graphics::GetDescriptorIndex(skyboxMesh->GetVertexBufferDescriptorHandle());

    // Per-frame camera data
    VertexShaderPerFrameData vsData{};
    vsData.view = camera->GetView();
    vsData.projection = camera->GetProjection();

    D3D12_GPU_DESCRIPTOR_HANDLE cbHandle =
        Graphics::FillNextConstantBufferAndGetGPUDescriptorHandle(
            &vsData, sizeof(VertexShaderPerFrameData));

    drawData.vsCBIndex = Graphics::GetDescriptorIndex(cbHandle);

    // Send indices to shader
    commandList->SetGraphicsRoot32BitConstants(
        0,
        sizeof(SkyDrawIndices) / sizeof(unsigned int),
        &drawData,
        0);

    // Bind geometry
    D3D12_INDEX_BUFFER_VIEW ibv = skyboxMesh->GetIndexBufferView();
    commandList->IASetIndexBuffer(&ibv);

    commandList->DrawIndexedInstanced(
        (UINT)skyboxMesh->GetIndexCount(), 1, 0, 0, 0);
}

