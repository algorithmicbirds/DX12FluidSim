#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>

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

private:
    void Init();
    void ShutDown();
    void ReleaseRTVHeaps();

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device &DeviceRef;
    ComPtr<ID3D12DescriptorHeap> RTVDescHeap;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RTVHandles;
};