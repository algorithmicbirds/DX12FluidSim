#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "D3D/Vertex.hpp"
#include <memory>

class DXPipeline;
class DXSwapchain;

class Renderer
{
public:
    Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device);
    ~Renderer();
    Renderer &operator=(const Renderer &) = delete;
    Renderer(const Renderer &) = delete;

public:
    void BeginFrame(ID3D12GraphicsCommandList7 *CmdList);
    void EndFrame(ID3D12GraphicsCommandList7 *CmdList);
    void CreateRTVAndDescHeap();
    void InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList);

private:
    void Init();
    void ReleaseRTVHeaps();
    ComPtr<ID3D12Resource2> CreateVertexBuffer(
        UINT64 SizeOfBufferInBytes, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES InitialResourceState
    );
    void CreateVertexBufferView();
    void TransitionResoure(
        ID3D12GraphicsCommandList7 *CmdList,
        ID3D12Resource *ResourceToTransition,
        D3D12_RESOURCE_STATES BeforeState,
        D3D12_RESOURCE_STATES AfterState
    );

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    ComPtr<ID3D12DescriptorHeap> RTVDescHeap;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RTVHandles;
    std::unique_ptr<DXPipeline> Pipeline;
    D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};

    // Gpu Only Buffer
    ComPtr<ID3D12Resource2> VertexBuffer_Default;

    // CPU Visible Buffer
    ComPtr<ID3D12Resource2> VertexBuffer_Upload;

    Vertex TriangleVertices[3] = {
        {-0.5f, -0.5f, 0.5f, 0.0f, 0.3f},
        {0.5f,  -0.5f, 0.0f, 0.5f, 1.0f},
        {0.0f,  0.5f,  0.0f, 1.0f, 0.5f}
    };
};