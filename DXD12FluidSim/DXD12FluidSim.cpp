#include "GlobInclude/WinInclude.hpp"
#include <iostream>
#include "DebugLayer/DebugLayer.hpp"
#include "D3D/DXContext.hpp"
#include "D3D/DXSwapchain.hpp"
#include "Window/Window.hpp"

int main(int argc, char **argv)
{
#ifdef _DEBUG
    DebugLayer Debug;
#endif // _DEBUG

    DXContext Context;
    Window Window;
    DXSwapChain SwapChain{Context, Window.GetHwnd()};
    while (!Window.ShouldClose())
    {
        Window.Update();
        ID3D12CommandList *CmdList = Context.InitCmdList();
        Context.DispatchCmdList();
        SwapChain.Present();
    }

    SwapChain.Flush(SwapChain.GetFrameCount());
    return 0;
}