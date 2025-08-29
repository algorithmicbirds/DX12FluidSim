#include "GlobInclude/WinInclude.hpp"
#include <iostream>
#include "DebugLayer/DebugLayer.hpp"
#include "D3D/DXContext.hpp"
#include "D3D/DXSwapchain.hpp"
#include "Window/Window.hpp"
#include "Renderer/Renderer.hpp"
#include "DebugLayer/DebugMacros.hpp"

int main(int argc, char **argv)
{
#ifdef _DEBUG
    DebugLayer Debug;
#endif // _DEBUG

    DXContext Context;
    Window Window;
    DXSwapchain Swapchain{Context, Window.GetHwnd()};

    Renderer Renderer{Swapchain, *Context.GetDevice()};

    while (!Window.ShouldClose())
    {
        Window.Update();
        if (Window.ShouldResize())
        {
            Context.Flush(Swapchain.GetFrameCount());
            Swapchain.Resize();
            Window.ClearResizeFlags();
            Renderer.CreateRTVAndDescHeap();
        }

        ID3D12GraphicsCommandList7 *CmdList = Context.InitCmdList();
        Renderer.BeginFrame(CmdList);
        Renderer.EndFrame(CmdList);
        Context.DispatchCmdList();
        Swapchain.Present();
    }

    Context.Flush(Swapchain.GetFrameCount());
    return 0;
}