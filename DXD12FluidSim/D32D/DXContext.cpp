#include "D32D/DXContext.hpp"
#include <stdexcept>

DXContext::DXContext()
{
    if (!Init())
    {
        throw std::runtime_error("Failed to make Context");
    }
}

DXContext::~DXContext() { ShutDown(); }

bool DXContext::Init()
{
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&Device))))
    {
        return false;
    }
    D3D12_COMMAND_QUEUE_DESC CmdQueueDesc{};
    CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    CmdQueueDesc.NodeMask = 0;
    CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    if (FAILED(Device->CreateCommandQueue(&CmdQueueDesc, IID_PPV_ARGS(&CommandQueue))))
    {
        return false;
    }

    if (FAILED(Device->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence))))
    {
        return false;
    }
    FenceEvent = CreateEvent(nullptr, false, false, nullptr);
    if (!FenceEvent)
    {
        return false;
    }

    if (FAILED(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CmdAlloc))))
    {
        return false;
    }

    if (FAILED(Device->CreateCommandList1(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&CmdList)
        )))
    {
        return false;
    }
    return true;
}

void DXContext::ShutDown()
{
    CmdList.Reset();
    CmdAlloc.Reset();
    if (FenceEvent)
    {
        CloseHandle(FenceEvent);
    }
    Fence.Reset();
    CommandQueue.Reset();
    Device.Reset();
}

void DXContext::SignalAndWait()
{
    CommandQueue->Signal(Fence.Get(), ++FenceValue);
    if (SUCCEEDED(Fence->SetEventOnCompletion(FenceValue, FenceEvent)))
    {
        if (WaitForSingleObject(FenceEvent, 2000) != WAIT_OBJECT_0)
        {
            std::exit(-1);
        }
    }
    else
    {
        std::exit(-1);
    }
}

ID3D12GraphicsCommandList1 *DXContext::InitCmdList()
{
    CmdAlloc->Reset();
    CmdList->Reset(CmdAlloc.Get(), nullptr);
    return CmdList.Get();
}

void DXContext::DispatchCmdList()
{
    CmdList->Close();
    ID3D12CommandList *Lists[] = {CmdList.Get()};
    CommandQueue->ExecuteCommandLists(1, Lists);
    SignalAndWait();
}
