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

    FenceEvent = CreateEvent(nullptr, false, false, nullptr);

    VALIDATE_PTR(FenceEvent);

    Frames.resize(FrameCount);
    for (size_t i = 0; i < FrameCount; ++i)
    {
        DX_VALIDATE(
            Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&Frames[i].CmdAlloc)),
            Frames[i].CmdAlloc
        );

        DX_VALIDATE(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Frames[i].Fence)), Frames[i].Fence);

        Frames[i].FenceValue = 0;
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
    if (FenceEvent)
    {
        CloseHandle(FenceEvent);
    }
    Frames.clear();
}

void DXContext::SignalAndWait()
{
    FrameData &Frame = Frames[CurrentFrameIndex];
    Frame.FenceValue++;
    DX_VALIDATE(CommandQueue->Signal(Frame.Fence.Get(), Frame.FenceValue), Signal);
    if (Frame.Fence->GetCompletedValue() < Frame.FenceValue)
    {
        DX_VALIDATE(Frame.Fence->SetEventOnCompletion(Frame.FenceValue, FenceEvent), SetEventOnCompletion);
        WAIT_FOR_HANDLE(FenceEvent, INFINITE);
    }
}

ID3D12GraphicsCommandList7 *DXContext::InitCmdList()
{
    FrameData &Frame = Frames[CurrentFrameIndex];
    Frame.CmdAlloc->Reset();
    CmdList->Reset(Frame.CmdAlloc.Get(), nullptr);
    return CmdList.Get();
}

void DXContext::DispatchCmdList()
{
    CmdList->Close();
    ID3D12CommandList *Lists[] = {CmdList.Get()};
    CommandQueue->ExecuteCommandLists(1, Lists);

    SignalAndWait();

    CurrentFrameIndex = (CurrentFrameIndex + 1) % FrameCount;
}

void DXContext::Flush(size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        SignalAndWait();
    }
}
