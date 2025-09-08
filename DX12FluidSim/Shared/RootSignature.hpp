#pragma once

#include "GlobInclude/WinInclude.hpp"

namespace RootSignature
{

ComPtr<ID3D12RootSignature> CreateGraphicsRootSig(ID3D12Device14 &Device);
ComPtr<ID3D12RootSignature> CreateComputeRootSig(ID3D12Device14 &Device);

} // namespace RootSignatures