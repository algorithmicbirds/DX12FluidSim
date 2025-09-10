#include "Window/UI.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "D3D/DXSwapchain.hpp"
#include "D3D/DXContext.hpp"
#include "Shared/SimInitials.hpp"

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

UI::UI(DXContext &Context, DXSwapchain &Swapchain, HWND Hwnd)
    : HwndRef(Hwnd), ContextRef(Context), SwapchainRef(Swapchain)
{
    InitializeImGUI();
}

UI::~UI()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UI::InitializeImGUI()
{
    ImGui::CreateContext();
    ImGuiIO IO = ImGui::GetIO();
    (void)IO;
    IO.Fonts->AddFontFromFileTTF(FONTS_PATH "Roboto-Regular.ttf", 16.0f);
    CreateDescHeap();

    ImGui_ImplWin32_Init(HwndRef);

    D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle = ImguiHeap->GetCPUDescriptorHandleForHeapStart();
    D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle = ImguiHeap->GetGPUDescriptorHandleForHeapStart();

    ImGui_ImplDX12_InitInfo Init_Info{};
    Init_Info.Device = ContextRef.GetDevice();
    Init_Info.NumFramesInFlight = static_cast<int>(SwapchainRef.GetFrameCount());
    Init_Info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    Init_Info.SrvDescriptorHeap = ImguiHeap.Get();
    Init_Info.CommandQueue = ContextRef.GetCommandQueue();

    Init_Info.LegacySingleSrvCpuDescriptor.ptr = CpuHandle.ptr;
    Init_Info.LegacySingleSrvGpuDescriptor.ptr = GpuHandle.ptr;

    ImGui_ImplDX12_Init(&Init_Info);

    ImGui::StyleColorsDark();
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
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void UI::RenderUI(ID3D12GraphicsCommandList7 *CmdList)
{
    NewFrame();

    ID3D12DescriptorHeap *heaps[] = {ImguiHeap.Get()};
    CmdList->SetDescriptorHeaps(_countof(heaps), heaps);

    ImGui::Begin("DX12FluidSim", nullptr, ImGuiWindowFlags_MenuBar);
    static float halfWidth = SimInitials::BoundingBoxWidth;
    static float halfHeight = SimInitials::BoundingBoxHeight;
    static float gravity = SimInitials::Gravity; 
    static float collisionDamping = SimInitials::CollisionDamping;
    static UINT pause = SimInitials::Pause;

    if (ImGui::SliderFloat("Width", &halfWidth, 0.5f, 3.5f))
    {
        OnWidthChanged.fire(halfWidth);
    }

    if (ImGui::SliderFloat("Height", &halfHeight, 0.25f, 2.0f))
    {
        OnHeightChanged.fire(halfHeight);
    }
    if (ImGui::SliderFloat("Gravity", &gravity, -30.0f, 30.0f))
    {
        OnGravityChanged.fire(gravity);
    }
    if (ImGui::SliderFloat("CollisonDamping", &collisionDamping, 0.0f, 1.0f))
    {
        OnCollisionDampingChanged.fire(collisionDamping);
    }
    if (ImGui::Button(pause ? "Pause" : "Play"))
    {
        pause ^= 1;             
        OnPauseToggled.fire(pause); 
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), CmdList);
}
