#include "GlobInclude/WinInclude.hpp"
#include <iostream>
#include "DebugLayer/DebugLayer.hpp"
#include "D32D/DXContext.hpp"

void TriggerLeak(ID3D12Device *device)
{
    ID3D12Resource *leakedResource = nullptr;
    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Width = 1024;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.SampleDesc.Count = 1;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = device->CreateCommittedResource(
        &heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&leakedResource)
    );
}


int main(int argc, char** argv) {
    DebugLayer Debug;
    DXContext Context;
    TriggerLeak(Context.GetDevice());

    Debug.ReportLiveObjects();

    return 0;
}