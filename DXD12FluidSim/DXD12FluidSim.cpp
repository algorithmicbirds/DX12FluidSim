#include "GlobInclude/WinInclude.hpp"
#include "DebugLayer/DebugLayer.hpp"
#include "D3D/DXContext.hpp"
#include "D3D/DXSwapchain.hpp"
#include "Window/Window.hpp"
#include "Renderer/Renderer.hpp"
#include "DebugLayer/DebugMacros.hpp"

void InitConsole()
{
    AllocConsole();

    FILE *fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
#ifdef _DEBUG
    DebugLayer Debug;
    InitConsole();
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
            Renderer.EndFrame(CmdList);
            Context.DispatchCmdList();
            Swapchain.Present();
        }

        Context.Flush(Swapchain.GetFrameCount());
    }
#ifdef _DEBUG
    Debug.ReportLiveObjects();
    std::cout << "Press any key to exit...\n";
    std::cin.get();
#endif // _DEBUG

    return 0;
}