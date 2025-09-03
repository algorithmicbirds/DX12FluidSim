#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "DebugLayer/DebugMacros.hpp"
#include "GlobInclude/Utils.hpp"
#include <span>

template <typename Vertex> class Mesh
{
public:
    Mesh(
        ID3D12Device14 &Device,
        ID3D12GraphicsCommandList7 *CmdList,
        std::span<const Vertex> Vertices,
        std::span<const uint16_t> Indices
    )
    {
        UINT VertexBufferSize = static_cast<UINT>(Vertices.size() * sizeof(Vertex));
        Utils::CreateUploadBuffer(
            Device, CmdList, VertexBufferSize, Vertices.data(), VertexBuffer_Default, VertexBuffer_Upload
        );

        VertexBufferView.BufferLocation = VertexBuffer_Default->GetGPUVirtualAddress();
        VertexBufferView.SizeInBytes = VertexBufferSize;
        VertexBufferView.StrideInBytes = sizeof(Vertex);

        UINT IndexBufferSize = static_cast<UINT>(Indices.size() * sizeof(uint16_t));
        Utils::CreateUploadBuffer(
            Device, CmdList, IndexBufferSize, Indices.data(), IndexBuffer_Default, IndexBuffer_Upload
        );

        IndexBufferView.BufferLocation = IndexBuffer_Default->GetGPUVirtualAddress();
        IndexBufferView.SizeInBytes = IndexBufferSize;
        IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
    }

    void Bind(ID3D12GraphicsCommandList7 *CmdList) const
    {
        CmdList->IASetVertexBuffers(0, 1, &VertexBufferView);
        CmdList->IASetIndexBuffer(&IndexBufferView);
        CmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    UINT GetIndexCount() const { return IndexBufferView.SizeInBytes / sizeof(uint16_t); }

    UINT GetVertexCount() const { return VertexBufferView.SizeInBytes / sizeof(Vertex); }

private:
    ComPtr<ID3D12Resource2> VertexBuffer_Default;
    ComPtr<ID3D12Resource2> VertexBuffer_Upload;
    D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};

    ComPtr<ID3D12Resource2> IndexBuffer_Default;
    ComPtr<ID3D12Resource2> IndexBuffer_Upload;
    D3D12_INDEX_BUFFER_VIEW IndexBufferView{};
};
