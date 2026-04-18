#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d12.h>
#include "Mesh.h"
#include "camera.h"


struct SkyDrawIndices
{
    unsigned int vsCBIndex;            // Constant buffer (camera)
    unsigned int vsVertexBufferIndex;  // Vertex buffer SRV index
    unsigned int psSkyboxIndex;        // Cubemap SRV index
};

class Skybox
{

private:

    // Index into the SRV descriptor heap
    unsigned int cubemapIndex;

    // Geometry
    std::shared_ptr<Mesh> skyboxMesh;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;

    void InitRenderStates();

public:
    Skybox(
        std::shared_ptr<Mesh> mesh,
        unsigned int cubemapIndex);

    ~Skybox();

    void Draw(
        ID3D12GraphicsCommandList* commandList,
        std::shared_ptr<Camera> camera);
};