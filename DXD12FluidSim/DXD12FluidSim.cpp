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
        Renderer.SetViewport(Swapchain.GetViewport());
        ID3D12GraphicsCommandList7 *CmdList = Context.InitCmdList();

        Renderer.InitializeBuffers(CmdList);
        Vertex CubeVertices[8] = {
            {-0.5f, -0.5f, -0.5f, 1,    0,    0   }, 
            {0.5f,  -0.5f, -0.5f, 0,    1,    0   }, 
            {0.5f,  0.5f,  -0.5f, 0,    0,    1   }, 
            {-0.5f, 0.5f,  -0.5f, 1,    1,    0   }, 
            {-0.5f, -0.5f, 0.5f,  0,    1,    1   }, 
            {0.5f,  -0.5f, 0.5f,  1,    0,    1   }, 
            {0.5f,  0.5f,  0.5f,  1,    1,    1   }, 
            {-0.5f, 0.5f,  0.5f,  0.5f, 0.5f, 0.5f}  
        };

        uint16_t CubeIndices[36] = {4, 5, 6, 4, 6, 7, 0, 2, 1, 0, 3, 2, 0, 7, 3, 0, 4, 7,
                                    1, 2, 6, 1, 6, 5, 3, 7, 6, 3, 6, 2, 0, 1, 5, 0, 5, 4};

        auto CubeObj = std::make_unique<GameObject>(GameObject::CreateGameObject());
        auto CubeMesh = std::make_shared<Mesh<Vertex>>(*Context.GetDevice(), CmdList, CubeVertices, CubeIndices);
        CubeObj->Mesh = CubeMesh;
        CubeObj->Transform.Rotation = {2.0f, 1.0f, 0.0f};
        Renderer.RegisterGameObject(CubeObj.get(), CmdList);

        float rotX = 0.0f, rotY = 0.0f;

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
                Renderer.OnResize(Swapchain.GetAspectRatio());
                Window.ClearResizeFlags();
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