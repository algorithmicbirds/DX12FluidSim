#pragma once
#include "GlobInclude/WinInclude.hpp"
#include <nano_signal_slot.hpp>

class UI
{
public:
    UI(class DXContext &Context, class DXSwapchain &Swapchain, HWND Hwnd);
    ~UI();

    void CreateDescHeap();
    void RenderUI(ID3D12GraphicsCommandList7 *CmdList, class Renderer &RendererRef);
    void NewFrame();

public:
    Nano::Signal<void(float)> OnHeightChanged;
    Nano::Signal<void(float)> OnWidthChanged;

private:
    void InitializeImGUI();

private:
    DXContext &ContextRef;
    HWND &HwndRef;
    DXSwapchain &SwapchainRef;
    ComPtr<ID3D12DescriptorHeap> ImguiHeap;
};