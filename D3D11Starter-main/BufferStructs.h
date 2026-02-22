#pragma once
#include <DirectXMath.h>


struct VertexShaderExternalData {
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

};

struct PixelShaderExternalData
{
    DirectX::XMFLOAT2 uvScale;
    DirectX::XMFLOAT2 uvOffset;

    unsigned int albedoIndex;
    unsigned int normalMapIndex;
    unsigned int roughnessIndex;
    unsigned int metalnessIndex;
};