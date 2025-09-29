#pragma once
#include "GlobInclude/WinInclude.hpp"
#include <nano_signal_slot.hpp>

class UI
{
public:
    UI(class DXContext &Context, class DXSwapchain &Swapchain, HWND Hwnd);
    ~UI();

    void CreateDescHeap();
    void RenderUI(ID3D12GraphicsCommandList7 *CmdList);
    void NewFrame();

public:
    Nano::Signal<void(float, float)> OnBBHeightOrWidthChanged;
    Nano::Signal<void(float)> OnGravityChanged;
    Nano::Signal<void(float)> OnCollisionDampingChanged;
    Nano::Signal<void(float)> OnStifnessConstantChanged;
    Nano::Signal<void(float)> OnViscosityCoeffecientChanged;
    Nano::Signal<void(float)> OnRestDensityChanged;
    Nano::Signal<void(float)> OnInteractionStrChanged;
    Nano::Signal<void(float)> OnInteractionRadChanged;
    Nano::Signal<void(UINT)> OnPauseToggled;
    Nano::Signal<void(DirectX::XMFLOAT4)> OnParticleBaseColorChanged;
    Nano::Signal<void(DirectX::XMFLOAT4)> OnParticleGlowColorChanged;
    void ToggleUI() { bRenderUIEnabled = !bRenderUIEnabled; }

private:
    void InitializeImGUI();

private:
    DXContext &ContextRef;
    HWND &HwndRef;
    DXSwapchain &SwapchainRef;
    ComPtr<ID3D12DescriptorHeap> ImguiHeap;
    bool bRenderUIEnabled = true;
};