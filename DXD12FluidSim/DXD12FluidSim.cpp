#include "GlobInclude/WinInclude.hpp"
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

    {
        DXContext Context;
        Window Window;
        DXSwapchain Swapchain{Context, Window.GetHwnd()};

        Renderer Renderer{Swapchain, *Context.GetDevice()};
        ID3D12GraphicsCommandList7 *CmdList = Context.InitCmdList(Swapchain.GetCurrentBackBufferIndex());
        Renderer.InitializeBuffers(CmdList);
        Context.DispatchCmdList();
        while (!Window.ShouldClose())
        {

#ifdef _DEBUG
            Debug.PrintLiveMessages();
#endif // _DEBUG

            Window.Update();
            if (Window.ShouldResize())
            {
                Context.Flush(Swapchain.GetFrameCount());
                Swapchain.Resize();
                Window.ClearResizeFlags();
                Renderer.CreateRTVAndDescHeap();
            }

            CmdList = Context.InitCmdList(Swapchain.GetCurrentBackBufferIndex());
            Renderer.BeginFrame(CmdList);
            Renderer.BindInputAssembler(CmdList);
            Renderer.EndFrame(CmdList);
            Context.DispatchCmdList();
            Swapchain.Present();
        }

        Context.Flush(Swapchain.GetFrameCount());
    }
    return 0;
}