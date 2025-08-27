#pragma once

#include "GlobInclude/WinInclude.hpp"

class DebugLayer
{
public:
    DebugLayer();
    ~DebugLayer();
    DebugLayer(const DebugLayer &) = delete;
    DebugLayer &operator=(const DebugLayer &) = delete;

     void ReportLiveObjects();

private:
    bool Init();
    void ShutDown();

private:
#ifdef _DEBUG
    ComPtr<ID3D12Debug6> D3D12Debug;
    ComPtr<IDXGIDebug1> DXGIDebug;
#endif // _DEBUG
};