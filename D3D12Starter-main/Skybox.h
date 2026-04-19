#pragma once

#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>

#include "Mesh.h"
#include "Camera.h"
#include "Graphics.h"

class Skybox
{
public:

    Skybox(
        const wchar_t* right,
        const wchar_t* left,
        const wchar_t* up,
        const wchar_t* down,
        const wchar_t* front,
        const wchar_t* back,
        std::shared_ptr<Mesh> cubeMesh,
        Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBytecode,
        Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBytecode);

    ~Skybox();

    void Draw(std::shared_ptr<Camera> camera);

private:

    unsigned int cubemapSRVIndex;

    std::shared_ptr<Mesh> cubeMesh;

    // PSO and root signature
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

    // Helper function
    void CreateSkyboxPSO(
        Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBytecode,
        Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBytecode);
};