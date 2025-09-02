#pragma once

#define NOMINMAX

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#ifdef _DEBUG
#include <dxgidebug.h>
#endif // _DEBUG
