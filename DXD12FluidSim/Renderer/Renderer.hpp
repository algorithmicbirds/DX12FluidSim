#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>
#include "D3D/Vertex.hpp"
#include <memory>
#include "Renderer/Camera.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/GameObject.hpp"
#include <unordered_map>

class DXPipeline;
class DXSwapchain;

struct GameObjectGPUData
{
    ComPtr<ID3D12Resource2> TransformBuffer_Default;
    ComPtr<ID3D12Resource2> TransformBuffer_Upload;
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
    void BeginFrame(ID3D12GraphicsCommandList7 *CmdList);
    void EndFrame(ID3D12GraphicsCommandList7 *CmdList);
    void CreateRTVAndDescHeap();
    void InitializeBuffers(ID3D12GraphicsCommandList7 *CmdList);
    void RegisterGameObject(GameObject *GameObj, ID3D12GraphicsCommandList7 *CmdList);

private:
    void Init();
    void ReleaseRTVHeaps();
    void UpdateCameraBuffer();
    void RenderGameObject(ID3D12GraphicsCommandList7 *CmdList);

private:
    DXSwapchain &SwapchainRef;
    ID3D12Device14 &DeviceRef;
    ComPtr<ID3D12DescriptorHeap> RTVDescHeap;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> RTVHandles;
    std::unique_ptr<DXPipeline> Pipeline;

    Camera Camera;
    ComPtr<ID3D12Resource2> CameraBuffer_Default;
    ComPtr<ID3D12Resource2> CameraBuffer_Upload;
    D3D12_GPU_VIRTUAL_ADDRESS CameraBufferGPUAddress;

    Vertex QuadVertices[4] = {
        {-0.5f, -0.5f, 1.0f, 0.0f, 0.0f}, // 0 bottom-left (red)
        {0.5f,  -0.5f, 0.0f, 1.0f, 0.0f}, // 1 bottom-right (green)
        {-0.5f, 0.5f,  0.0f, 0.0f, 1.0f}, // 2 top-left (blue)
        {0.5f,  0.5f,  1.0f, 1.0f, 0.0f}  // 3 top-right (yellow)
    };

    uint16_t QuadIndices[6] = {
        0,
        1,
        2, // first triangle
        2,
        1,
        3 // second triangle
    };

    std::unique_ptr<Mesh<Vertex>> QuadMesh;
    std::unordered_map<GameObject *, GameObjectGPUData> GameObjectResources;
    std::vector<GameObject *> RegisteredObjects;
};