#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include <span>
#include <functional>
#include "DebugLayer/DebugMacros.hpp"
#include "Shared/Utils.hpp"

template <typename Vertex> class CPUMesh;

class GPUMesh
{
public:
    template <typename Vertex>
    void UploadToGPU(ID3D12Device14 &Device, ID3D12GraphicsCommandList7 *CmdList, const CPUMesh<Vertex> &CPU);

    void Bind(ID3D12GraphicsCommandList *CmdList) const
    {
        CmdList->IASetVertexBuffers(0, 1, &VertexBufferView);
        CmdList->IASetIndexBuffer(&IndexBufferView);
        CmdList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    UINT GetVertexCount() const { return VertexBufferView.SizeInBytes / VertexBufferView.StrideInBytes; }
    UINT GetIndexCount() const { return IndexBufferView.SizeInBytes / sizeof(uint16_t); }

private:
    ComPtr<ID3D12Resource2> VertexBuffer_Default;
    ComPtr<ID3D12Resource2> VertexBuffer_Upload;
    D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};

    ComPtr<ID3D12Resource2> IndexBuffer_Default;
    ComPtr<ID3D12Resource2> IndexBuffer_Upload;
    D3D12_INDEX_BUFFER_VIEW IndexBufferView{};
};

class CPUMeshBase
{
public:
    virtual ~CPUMeshBase() = default;
    std::function<void(GPUMesh *, ID3D12Device14 &, ID3D12GraphicsCommandList7 *)> UploadFunc;
};

template <typename Vertex> class CPUMesh : public CPUMeshBase
{
public:
    CPUMesh(std::span<const Vertex> Vertices, std::span<const uint16_t> Indices)
    {
        m_Vertices.assign(Vertices.begin(), Vertices.end());
        m_Indices.assign(Indices.begin(), Indices.end());

        UploadFunc = [this](GPUMesh *GPU, ID3D12Device14 &Device, ID3D12GraphicsCommandList7 *CmdList)
        { GPU->UploadToGPU(Device, CmdList, *this); };
    }

    std::vector<Vertex> m_Vertices;
    std::vector<uint16_t> m_Indices;
};

template <typename Vertex>
void GPUMesh::UploadToGPU(ID3D12Device14 &Device, ID3D12GraphicsCommandList7 *CmdList, const CPUMesh<Vertex> &CPU)
{
    UINT VertexBufferSize = static_cast<UINT>(CPU.m_Vertices.size() * sizeof(Vertex));
    Utils::CreateUploadBuffer(
        Device, CmdList, VertexBufferSize, CPU.m_Vertices.data(), VertexBuffer_Default, VertexBuffer_Upload
    );

    VertexBufferView.BufferLocation = VertexBuffer_Default->GetGPUVirtualAddress();
    VertexBufferView.SizeInBytes = VertexBufferSize;
    VertexBufferView.StrideInBytes = sizeof(Vertex);

    UINT IndexBufferSize = static_cast<UINT>(CPU.m_Indices.size() * sizeof(uint16_t));
    Utils::CreateUploadBuffer(
        Device, CmdList, IndexBufferSize, CPU.m_Indices.data(), IndexBuffer_Default, IndexBuffer_Upload
    );

    IndexBufferView.BufferLocation = IndexBuffer_Default->GetGPUVirtualAddress();
    IndexBufferView.SizeInBytes = IndexBufferSize;
    IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
}
