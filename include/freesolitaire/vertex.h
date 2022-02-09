#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct VertexPosColor {
    VertexPosColor() = default;
    VertexPosColor(const VertexPosColor&) = default;
    VertexPosColor& operator=(const VertexPosColor&) = default;
    VertexPosColor(VertexPosColor&&) = default;
    VertexPosColor& operator=(VertexPosColor&&) = default;

    constexpr VertexPosColor(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT4& _color) :
        pos(_pos), color(_color) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosTex {
    VertexPosTex() = default;
    VertexPosTex(const VertexPosTex&) = default;
    VertexPosTex& operator=(const VertexPosTex&) = default;
    VertexPosTex(VertexPosTex&&) = default;
    VertexPosTex& operator=(VertexPosTex&&) = default;

    constexpr VertexPosTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT2& _tex) :
        pos(_pos), tex(_tex) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 tex;
    static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};
