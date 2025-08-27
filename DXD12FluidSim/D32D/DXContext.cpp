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
    if (SUCCEEDED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&Device))))
    {
        D3D12_COMMAND_QUEUE_DESC CmdQueueDesc{};
        CmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        CmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        CmdQueueDesc.NodeMask = 0;
        CmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        if (SUCCEEDED(Device->CreateCommandQueue(&CmdQueueDesc, IID_PPV_ARGS(&CommandQueue))))
        {
            if (SUCCEEDED(Device->CreateFence(FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence))))
            {
                return true;
            }
        }
    }

    return false;
}

void DXContext::ShutDown()
{
    Fence.Reset();
    CommandQueue.Reset();
    Device.Reset();
}
