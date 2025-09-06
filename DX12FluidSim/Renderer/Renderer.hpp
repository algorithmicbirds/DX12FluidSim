#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "D3D/Vertex.hpp"
#include <memory>
#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/GameObject.hpp"
#include <unordered_map>

class DXGraphicsPipeline;
class DXSwapchain;
class DXComputePipeline;

struct GameObjectGPUData
{
    ComPtr<ID3D12Resource2> TransformBuffer_Default;
    ComPtr<ID3D12Resource2> TransformBuffer_Upload;
    void *MappedPtr = nullptr;
    D3D12_GPU_VIRTUAL_ADDRESS GPUAddress;
};

struct CameraBufferConstants
{
    DirectX::XMMATRIX ViewProjection;
};

struct TransformConstants
{
    DirectX::XMFLOAT4X4 ModelMatrix;
};

class Renderer
{
public:
    Renderer(DXSwapchain &Swapchain, ID3D12Device14 &Device);
    ~Renderer();
    Renderer &operator=(const Renderer &) = delete;
    Renderer(const Renderer &) = delete;

public:
    void RenderFrame(ID3D12GraphicsCommandList7 *CmdList);
    void InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList);
    void RegisterGameObject(GameObject *GameObj, ID3D12GraphicsCommandList7 *CmdList);
    void OnResize(float NewAspectRatio);
    void SetViewport(D3D12_VIEWPORT NewVP) { Viewport = NewVP; }
  

private:
    void UpdateCameraBuffer();
    void RenderGameObject(ID3D12GraphicsCommandList7 *CmdList);

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    D3D12_VIEWPORT Viewport{};

    std::unique_ptr<DXGraphicsPipeline> GraphicsPipeline;
    std::unique_ptr<DXComputePipeline> ComputePipeline;

    Camera Camera;
    ComPtr<ID3D12Resource2> CameraBuffer_Default;
    ComPtr<ID3D12Resource2> CameraBuffer_Upload;
    D3D12_GPU_VIRTUAL_ADDRESS CameraBufferGPUAddress;

    std::unordered_map<GameObject *, GameObjectGPUData> GameObjectResources;
    std::vector<GameObject *> RegisteredObjects;
};