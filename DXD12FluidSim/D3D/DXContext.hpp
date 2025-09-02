#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>

struct FrameData
{
    ComPtr<ID3D12CommandAllocator> CmdAlloc;
    ComPtr<ID3D12Fence1> Fence;
    UINT64 FenceValue;
};

class DXContext
{
public:
    DXContext();
    ~DXContext();
    DXContext &operator=(const DXContext &) = delete;
    DXContext(const DXContext &) = delete;

    inline ID3D12Device14 *GetDevice() const { return Device.Get(); }
    inline ID3D12CommandQueue *GetCommandQueue() const { return CommandQueue.Get(); }
    inline IDXGIFactory7 *GetDXGIFactory() const { return DXGIFactory.Get(); }

public:
    void SignalAndWait();
    ID3D12GraphicsCommandList7 *InitCmdList();
    void DispatchCmdList();
    void Flush(size_t count);

private:
    bool Init();
    void ShutDown();

private:
    ComPtr<ID3D12Device14> Device;
    ComPtr<ID3D12CommandQueue> CommandQueue;
    std::vector<FrameData> Frames;
    ComPtr<ID3D12GraphicsCommandList7> CmdList;
    ComPtr<IDXGIFactory7> DXGIFactory;
    HANDLE FenceEvent = nullptr;
    UINT CurrentFrameIndex = 0;
    size_t FrameCount = 3;
};