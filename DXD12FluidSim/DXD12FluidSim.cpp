#include "GlobInclude/WinInclude.hpp"
#include <iostream>
#include "DebugLayer/DebugLayer.hpp"
#include "D32D/DXContext.hpp"
#include "Window/Window.hpp"

int main(int argc, char **argv)
{
    DebugLayer Debug;
    DXContext Context;
    Window Window;

    while (!Window.ShouldClose())
    {
        Window.Update();
        ID3D12CommandList *CmdList = Context.InitCmdList(); 
        Context.DispatchCmdList();
    }


    Debug.ReportLiveObjects();

    return 0;
}