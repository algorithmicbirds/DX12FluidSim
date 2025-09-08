#pragma once
#include "GlobInclude/WinInclude.hpp"

class UI
{
public:
    UI(class DXContext &Context, class DXSwapchain &Swapchain, HWND Hwnd);
    ~UI();

    void CreateDescHeap();
    void RenderUI(ID3D12GraphicsCommandList7 *CmdList, class Renderer &RendererRef);
    void NewFrame();

private:
    void InitializeImGUI();

private:
    DXContext &ContextRef;
    HWND &HwndRef;
    DXSwapchain &SwapchainRef;
    ComPtr<ID3D12DescriptorHeap> ImguiHeap;
};