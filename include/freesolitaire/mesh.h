#pragma once

#include <vector>
#include <string>
#include <map>
#include "vertex.h"

namespace FreeSolitaire {
    template<typename VertexType = VertexPosTex, typename IndexType = DWORD>
    struct MeshData {
        std::vector<VertexType> vertexVec;
        std::vector<IndexType> indexVec;

        MeshData() {
            static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4, "The size of IndexType must be 2 bytes or 4 bytes!");
            static_assert(std::is_unsigned<IndexType>::value, "IndexType must be unsigned integer!");
        }
    };

    template<typename VertexType = VertexPosTex, typename IndexType = DWORD>
    MeshData<VertexType, IndexType> CreatePlane(const DirectX::XMFLOAT2& planeSize,
        const DirectX::XMFLOAT2& maxTexCoord = { 1.0f, 1.0f }, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
    template<typename VertexType = VertexPosTex, typename IndexType = DWORD>
    MeshData<VertexType, IndexType> CreatePlane(float width = 10.0f, float height = 10.0f, float texU = 1.0f, float texV = 1.0f,
        const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    namespace Internal {
        struct VertexData {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT4 color;
            DirectX::XMFLOAT2 tex;
        };

        template<typename VertexType>
        inline void insertVertexElement(VertexType& vertexDst, const VertexData& vertexSrc) {
            static std::string semanticName;
            static const std::map<std::string, std::pair<size_t, size_t>> semanticSizeMap = {
                {"POSITION", std::pair<size_t, size_t>(0, 12)},
                {"COLOR", std::pair<size_t, size_t>(12, 28)},
                {"TEXCOORD", std::pair<size_t, size_t>(28, 36)}
            };

            for (size_t i = 0; i < ARRAYSIZE(VertexType::inputLayout); ++i) {
                semanticName = VertexType::inputLayout[i].SemanticName;
                const auto& range = semanticSizeMap.at(semanticName);
                memcpy_s(reinterpret_cast<char*>(&vertexDst) + VertexType::inputLayout[i].AlignedByteOffset,
                    range.second - range.first,
                    reinterpret_cast<const char*>(&vertexSrc) + range.first,
                    range.second - range.first);
            }
        }
    }

    template<typename VertexType, typename IndexType>
    inline MeshData<VertexType, IndexType> CreatePlane(const DirectX::XMFLOAT2& planeSize,
        const DirectX::XMFLOAT2& maxTexCoord, const DirectX::XMFLOAT4& color) {
        return CreatePlane<VertexType, IndexType>(planeSize.x, planeSize.y, maxTexCoord.x, maxTexCoord.y, color);
    }

    template<typename VertexType, typename IndexType>
    inline MeshData<VertexType, IndexType> CreatePlane(float width, float height, float texU, float texV, const DirectX::XMFLOAT4& color) {
        using namespace DirectX;

        MeshData<VertexType, IndexType> meshData;
        meshData.vertexVec.resize(4);

        Internal::VertexData vertexData;
        UINT vIndex = 0;

        vertexData = { XMFLOAT3(-width / 2, 0.0f, -height / 2), color, XMFLOAT2(0.0f, texV) };
        Internal::insertVertexElement(meshData.vertexVec[vIndex++], vertexData);

        vertexData = { XMFLOAT3(-width / 2, 0.0f, height / 2), color, XMFLOAT2(0.0f, 0.0f) };
        Internal::insertVertexElement(meshData.vertexVec[vIndex++], vertexData);

        vertexData = { XMFLOAT3(width / 2, 0.0f, height / 2), color, XMFLOAT2(texU, 0.0f) };
        Internal::insertVertexElement(meshData.vertexVec[vIndex++], vertexData);

        vertexData = { XMFLOAT3(width / 2, 0.0f, -height / 2), color, XMFLOAT2(texU, texV) };
        Internal::insertVertexElement(meshData.vertexVec[vIndex++], vertexData);

        meshData.indexVec = { 0, 1, 2, 2, 3, 0 };
        return meshData;
    }
}
