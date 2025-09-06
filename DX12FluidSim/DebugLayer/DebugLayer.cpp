#include "DebugLayer.hpp"

DebugLayer::DebugLayer() { Init(); }
DebugLayer::~DebugLayer() { ShutDown(); }

bool DebugLayer::Init()
{
#ifdef _DEBUG
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&D3D12Debug))))
    {
        D3D12Debug->EnableDebugLayer();
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DXGIDebug))))
        {
            DXGIDebug->EnableLeakTrackingForThread();
            return true;
        }
    }
#endif
    return false;
}

void DebugLayer::ShutDown()
{
#ifdef _DEBUG
    ReportLiveObjects();
#endif
}

void DebugLayer::PrintLiveMessages()
{
#ifdef _DEBUG
    ComPtr<IDXGIInfoQueue> infoQueue;
    if (FAILED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&infoQueue))))
        return;

    UINT64 numMessages = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
    for (UINT64 i = 0; i < numMessages; ++i)
    {
        SIZE_T len = 0;
        infoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &len);
        std::vector<char> bytes(len);
        auto *msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE *>(bytes.data());
        infoQueue->GetMessage(DXGI_DEBUG_ALL, i, msg, &len);
        std::wcout << L"[D3D12] " << msg->pDescription << L"\n";
    }
    infoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
#endif
}

void DebugLayer::ReportLiveObjects()
{
#ifdef _DEBUG
    if (!DXGIDebug)
        return;

    std::wcout << L"=== DXGI Live Objects Report ===\n";
    DXGIDebug->ReportLiveObjects(
        DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL)
    );

    ComPtr<IDXGIInfoQueue> infoQueue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&infoQueue))))
    {
        UINT64 numMessages = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
        for (UINT64 i = 0; i < numMessages; ++i)
        {
            SIZE_T len = 0;
            infoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &len);
            std::vector<char> bytes(len);
            auto *msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE *>(bytes.data());
            infoQueue->GetMessage(DXGI_DEBUG_ALL, i, msg, &len);
            std::wcout << L"[DXGI] " << msg->pDescription << L"\n";
        }
        infoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
    }

    std::wcout << L"=== End of Report ===\n";
#endif
}
