#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "DebugLayer/DebugMacros.hpp"
#include "Shared/Utils.hpp"

class FluidHeapDescriptor
{
public:
    // for now just harcoding no of desc
    // i guess will look in future to make it dynamic manager
    FluidHeapDescriptor(ID3D12Device14 &Device) : DeviceRef(Device)
    {
        D3D12_DESCRIPTOR_HEAP_DESC HeapDesc{};
        HeapDesc.NumDescriptors = 19;
        HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        DX_VALIDATE(Device.CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&DescriptorHeap)), DescriptorHeap);
    };
    inline ID3D12DescriptorHeap *GetDescriptorHeap() const { return DescriptorHeap.Get(); }
    inline D3D12_GPU_DESCRIPTOR_HANDLE
    AllocateDescriptor(DescriptorType DescType, ComPtr<ID3D12Resource2> DefaultBuffer, UINT NumOfElems, UINT Stride)
    {
        D3D12_GPU_DESCRIPTOR_HANDLE Desc = Utils::CreateBufferDescriptor(
            DeviceRef, DescType, DefaultBuffer, DescriptorHeap, NumOfElems, Stride, NextIndex
        );
        NextIndex++;
        return Desc;
    }

private:
    ComPtr<ID3D12DescriptorHeap> DescriptorHeap;
    ID3D12Device14 &DeviceRef;
    UINT NextIndex = 0;
};