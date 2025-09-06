#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <iostream>
#include <vector>

using Microsoft::WRL::ComPtr;

class DebugLayer
{
public:
    DebugLayer();
    ~DebugLayer();

    bool Init();
    void ShutDown();

    void PrintLiveMessages();
    void ReportLiveObjects();

private:
#ifdef _DEBUG
    ComPtr<ID3D12Debug> D3D12Debug;
    ComPtr<IDXGIDebug1> DXGIDebug;
#endif

};

