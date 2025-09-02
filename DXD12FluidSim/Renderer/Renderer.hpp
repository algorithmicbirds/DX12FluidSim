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
    ComPtr<ID3D12Resource2>
    CreateBuffer(UINT64 SizeOfBufferInBytes, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES InitialResourceState);
    void TransitionResoure(
        ID3D12GraphicsCommandList7 *CmdList,
        ID3D12Resource *ResourceToTransition,
        D3D12_RESOURCE_STATES BeforeState,
        D3D12_RESOURCE_STATES AfterState
    );
    void CreateUploadBuffer(
        ID3D12GraphicsCommandList7 *CmdList,
        UINT BufferSize,
        const void *CPUData,
        ComPtr<ID3D12Resource2> &DefaultBuffer,
        ComPtr<ID3D12Resource2> &UploadBuffer
    );

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    ComPtr<ID3D12DescriptorHeap> RTVDescHeap;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RTVHandles;
    std::unique_ptr<DXPipeline> Pipeline;
    D3D12_VERTEX_BUFFER_VIEW VertexBufferView{};
    D3D12_INDEX_BUFFER_VIEW IndexBufferView{};

    // Gpu Only Buffer
    ComPtr<ID3D12Resource2> VertexBuffer_Default;
    ComPtr<ID3D12Resource2> IndexBuffer_Default;

    // CPU Visible Buffer
    ComPtr<ID3D12Resource2> VertexBuffer_Upload;
    ComPtr<ID3D12Resource2> IndexBuffer_Upload;

    Vertex QuadVertices[4] = {
        {-0.5f, -0.5f, 1.0f, 0.0f, 0.0f}, // 0 bottom-left (red)
        {0.5f,  -0.5f, 0.0f, 1.0f, 0.0f}, // 1 bottom-right (green)
        {-0.5f, 0.5f,  0.0f, 0.0f, 1.0f}, // 2 top-left (blue)
        {0.5f,  0.5f,  1.0f, 1.0f, 0.0f}  // 3 top-right (yellow)
    };

    uint16_t QuadIndices[6] = {
        0,
        1,
        2, // first triangle
        2,
        1,
        3 // second triangle
    };
};