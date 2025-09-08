#include "GlobInclude/WinInclude.hpp"
#include "DebugLayer/DebugLayer.hpp"
#include "D3D/DXContext.hpp"
#include "D3D/DXSwapchain.hpp"
#include "Window/Window.hpp"
#include "Renderer/Renderer.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "Renderer/Scene.hpp"
#include "FluidSimulation/FluidSimulation.hpp"
#include "Window/UI.hpp"

#ifdef _DEBUG
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
#endif // _DEBUG

void SetViewPort(DXSwapchain &Swapchain, Renderer &Renderer)
{
    D3D12_VIEWPORT VP = {};
    float WindowAR = static_cast<float>(Swapchain.GetWidth()) / Swapchain.GetHeight();
    float TargetAR = 16.0f / 9.0f;

    if (WindowAR > TargetAR)
    {
        VP.Width = TargetAR / WindowAR * Swapchain.GetWidth();
        VP.TopLeftX = (Swapchain.GetWidth() - VP.Width) / 2.0f;
        VP.Height = static_cast<float>(Swapchain.GetHeight());
        VP.TopLeftY = 0.0f;
    }
    else // window too tall
    {
        VP.Height = WindowAR / TargetAR * Swapchain.GetHeight();
        VP.TopLeftY = (Swapchain.GetHeight() - VP.Height) / 2.0f;
        VP.Width = static_cast<float>(Swapchain.GetWidth());
        VP.TopLeftX = 0.0f;
    }
    VP.MinDepth = 0.0f;
    VP.MaxDepth = 1.0f;

    Renderer.SetViewport(VP);
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
        Renderer.SetViewport(Swapchain.GetViewport());
        ID3D12GraphicsCommandList7 *CmdList = Context.InitCmdList();

        Renderer.InitializeBuffers(CmdList);

        Scene Scene{Renderer, *Context.GetDevice()};

        FluidSimulation FluidSim{Scene, Renderer};
        UI UI{Context, Swapchain, Window.GetHwnd()};

        Scene.FlushToRenderer(CmdList);

        Context.DispatchCmdList();
        while (!Window.ShouldClose())
        {
#ifdef _DEBUG
            Debug.PrintLiveMessages();
#endif // _DEBUG

            Window.UpdateMsg();
            Window.UpdateKeyBoard();
            if (Window.KeyPressed(VK_F11))
            {
                Window.FullScreenFlipFlop();
            }
            if (Window.KeyPressed(0x41))
            {
                printf("A pressed");
            }
            if (Window.KeyPressed(0x57))
            {
                printf("W pressed");
            }
            Window.ResetKeyBoardState();


            if (Window.ShouldResize())
            {
                Context.Flush(Swapchain.GetFrameCount());
                Swapchain.Resize();
                Renderer.OnResize(Swapchain.GetAspectRatio());
                Window.ClearResizeFlags();
                SetViewPort(Swapchain, Renderer);
            }

            float DeltaTime = Window.GetDeltaTimeSeconds();
            CmdList = Context.InitCmdList();
            Swapchain.TransitionCurrentToRT(CmdList);
            Renderer.RenderFrame(CmdList, DeltaTime);
            UI.RenderUI(CmdList);
            Swapchain.TransitionRTToPresent(CmdList);
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