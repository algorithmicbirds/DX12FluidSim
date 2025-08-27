#pragma once

#include "GlobInclude/WinInclude.hpp"

class DXContext
{
public:
    DXContext();
    ~DXContext();

    bool Init();
    void ShutDown();

    inline ID3D12Device *GetDevice() const { return Device.Get(); }
    inline ID3D12CommandQueue *GetCommandQueue() const { return CommandQueue.Get(); }
    inline ID3D12Fence *GetFence() const { return Fence.Get(); }

public:
    DXContext(const DXContext &) = delete;
    DXContext &operator=(const DXContext &) = delete;

private:
    ComPtr<ID3D12Device14> Device;
    ComPtr<ID3D12CommandQueue> CommandQueue;
    ComPtr<ID3D12Fence1> Fence;
    UINT64 FenceValue = 0;
};