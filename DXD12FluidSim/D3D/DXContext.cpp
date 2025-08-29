#include "D3D/DXContext.hpp"
#include "DebugLayer/DebugMacros.hpp"

DXContext::DXContext() { Init(); }

DXContext::~DXContext() { ShutDown(); }

bool DXContext::Init()
{

    DX_VALIDATE(CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory)), DXGIFactory);
    DX_VALIDATE(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&Device)), Device);

    D3D12_COMMAND_QUEUE_DESC CmdQueueDesc{};
    CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    CmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
    CmdQueueDesc.NodeMask = 0;
    CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    DX_VALIDATE(Device->CreateCommandQueue(&CmdQueueDesc, IID_PPV_ARGS(&CommandQueue)), CommandQueue);
    DX_VALIDATE(Device->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)), Fence);

    FenceEvent = CreateEvent(nullptr, false, false, nullptr);

    VALIDATE_PTR(FenceEvent);

    CmdAllocs.resize(FrameCount);
    for (size_t i = 0; i < FrameCount; ++i)
    {
        DX_VALIDATE(
            Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CmdAllocs[i])),
            CmdAllocs[i]
        );
    }

    DX_VALIDATE(
        Device->CreateCommandList1(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&CmdList)
        ),
        CmdList
    );
    return true;
}

void DXContext::ShutDown()
{
    CmdList.Reset();
    CmdAllocs.clear();
    if (FenceEvent)
    {
        CloseHandle(FenceEvent);
    }
    Fence.Reset();
    CommandQueue.Reset();
    Device.Reset();
    DXGIFactory.Reset();
}

void DXContext::SignalAndWait()
{
    DX_VALIDATE(CommandQueue->Signal(Fence.Get(), ++FenceValue), Signal);
    DX_VALIDATE(Fence->SetEventOnCompletion(FenceValue, FenceEvent), SetEventOnCompletion);
    WAIT_FOR_HANDLE(FenceEvent, 2000);
}

ID3D12GraphicsCommandList7 *DXContext::InitCmdList(UINT CurrentBufferIndex)
{
    CmdAllocs[CurrentBufferIndex]->Reset();
    CmdList->Reset(CmdAllocs[CurrentBufferIndex].Get(), nullptr);
    return CmdList.Get();
}

void DXContext::DispatchCmdList()
{
    CmdList->Close();
    ID3D12CommandList *Lists[] = {CmdList.Get()};
    CommandQueue->ExecuteCommandLists(1, Lists);
    SignalAndWait();
}

void DXContext::Flush(size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        SignalAndWait();
    }
}
