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
        auto QuadObj = std::make_unique<GameObject>(GameObject::CreateGameObject());

        Renderer Renderer{Swapchain, *Context.GetDevice()};
        ID3D12GraphicsCommandList7 *CmdList = Context.InitCmdList();
        
        Renderer.InitializeBuffers(CmdList);
        Vertex QuadVertices[4] = {
            {-0.5f, -0.5f, 1.0f, 0.0f, 0.0f}, // 0 bottom-left (red)
            {0.5f,  -0.5f, 0.0f, 1.0f, 0.0f}, // 1 bottom-right (green)
            {-0.5f, 0.5f,  0.0f, 0.0f, 1.0f}, // 2 top-left (blue)
            {0.5f,  0.5f,  1.0f, 1.0f, 0.0f}  // 3 top-right (yellow)
        };

        uint16_t QuadIndices[6] = {
            0,
            1,
            2, // first triangle
            2,
            1,
            3 // second triangle
        };
        auto QuadMesh = std::make_shared<Mesh<Vertex>>(*Context.GetDevice(), CmdList, QuadVertices, QuadIndices);
        QuadObj->Mesh = QuadMesh;
        QuadObj->Transform.Rotation = {0.0f, 0.0f, 0.0f};
        Renderer.RegisterGameObject(QuadObj.get(), CmdList);


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
            }
            //QuadObj->Transform.Rotation = {1.0f, 1.0f, 0.0f};
            CmdList = Context.InitCmdList();
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