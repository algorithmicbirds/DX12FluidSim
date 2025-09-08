#include "Window/UI.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXSwapchain.hpp"
#include "D3D/DXContext.hpp"

#include <NsCore/Noesis.h>
#include <NsCore/ReflectionImplementEmpty.h>
#include <NsCore/RegisterComponent.h>



UI::UI(DXContext &Context, DXSwapchain &Swapchain, HWND Hwnd)
    : HwndRef(Hwnd), ContextRef(Context), SwapchainRef(Swapchain)
{
    InitializeImgui();
}

UI::~UI() {
}

void UI::InitializeImgui()
{
   
}

void UI::CreateDescHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC Desc{};
    Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    Desc.NodeMask = 0;
    Desc.NumDescriptors = 1;
    Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    DX_VALIDATE(ContextRef.GetDevice()->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&ImguiHeap)), ImguiHeap);
}

void UI::NewFrame()
{
   
}

void UI::RenderUI(ID3D12GraphicsCommandList7 *CmdList)
{
    NewFrame();

}
