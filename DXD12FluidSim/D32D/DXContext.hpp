#pragma once

#include "GlobInclude/WinInclude.hpp"

class DXContext
{
public:
    DXContext();
    ~DXContext();
    DXContext &operator=(const DXContext &) = delete;
    DXContext(const DXContext &) = delete;

    inline ID3D12Device *GetDevice() const { return Device.Get(); }
    inline ID3D12CommandQueue *GetCommandQueue() const { return CommandQueue.Get(); }
    inline ID3D12Fence *GetFence() const { return Fence.Get(); }

public:
    void SignalAndWait();
    ID3D12GraphicsCommandList1 *InitCmdList();
    void DispatchCmdList();

private:
    bool Init();
    void ShutDown();

private:
    ComPtr<ID3D12Device14> Device;
    ComPtr<ID3D12CommandQueue> CommandQueue;
    ComPtr<ID3D12Fence1> Fence;
    ComPtr<ID3D12CommandAllocator> CmdAlloc;
    ComPtr<ID3D12GraphicsCommandList1> CmdList;
    HANDLE FenceEvent = nullptr;
    UINT64 FenceValue = 0;
};