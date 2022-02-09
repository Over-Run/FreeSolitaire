#include "Header.hlsli"

cbuffer MvpConstantBuffer : register(b0)
{
    matrix mWorld;
    matrix view;
    matrix proj;
}

VertexOut main(VertexIn vIn)
{
    VertexOut vOut;
    vOut.posH = mul(float4(vIn.pos, 1.0f), mWorld);
    vOut.posH = mul(vOut.posH, view);
    vOut.posH = mul(vOut.posH, proj);
    vOut.color = vIn.color;
    return vOut;
}
