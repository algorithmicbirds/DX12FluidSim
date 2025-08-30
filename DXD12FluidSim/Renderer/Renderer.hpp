#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "D3D/Vertex.hpp"

class DXPipeline;
class DXSwapchain;

class Renderer
{
public:
    Renderer(DXSwapchain &Swapchain, ID3D12Device& Device);
    ~Renderer();
    Renderer &operator=(const Renderer &) = delete;
    Renderer(const Renderer &) = delete;

public:
    void BeginFrame(ID3D12GraphicsCommandList7 *CmdList);
    void EndFrame(ID3D12GraphicsCommandList7 *CmdList);
    void CreateRTVAndDescHeap();
    void InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList);
    void BindInputAssembler(ID3D12GraphicsCommandList7 *CmdList);

private:
    void Init();
    void ShutDown();
    void ReleaseRTVHeaps();
    ComPtr<ID3D12Resource2> CreateVertexBuffer(
        UINT64 SizeOfBufferInBytes, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_STATES InitialResourceState
    );

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device &DeviceRef;
    ComPtr<ID3D12DescriptorHeap> RTVDescHeap;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RTVHandles;

    //Gpu Only Buffer
    ComPtr<ID3D12Resource2> VertexBuffer_Default;

    //CPU Visible Buffer
    ComPtr<ID3D12Resource2> VertexBuffer_Upload;

      Vertex TriangleVertices[3] = {
        {-1.0f, -1.0f},
        {-1.0f, 0.0f },
        {0.0f,  1.0f }
    };  

};