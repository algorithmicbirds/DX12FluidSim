#pragma once

#include "GlobInclude/WinInclude.hpp"

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

    void Init();
private:
    void ShutDown();

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device &DeviceRef;
    ComPtr<ID3D12DescriptorHeap> RTVDescHeap;
};