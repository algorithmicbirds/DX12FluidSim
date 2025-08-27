#include "DebugLayer/DebugLayer.hpp"
#include <iostream>
#include <vector>

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
    ComPtr<IDXGIInfoQueue> infoQueue;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&infoQueue))))
    {
        UINT64 numMessages = infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
        for (UINT64 i = 0; i < numMessages; ++i)
        {
            SIZE_T messageLength = 0;
            infoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
            std::vector<char> bytes(messageLength);
            DXGI_INFO_QUEUE_MESSAGE *msg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE *>(bytes.data());
            infoQueue->GetMessage(DXGI_DEBUG_ALL, i, msg, &messageLength);
            std::wcout << L"[DXGI] " << msg->pDescription << L"\n";
        }
        std::wcout << L"=== End of Report ===\n";
        infoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
    }
    DXGIDebug.Reset();
    D3D12Debug.Reset();
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
#endif
}