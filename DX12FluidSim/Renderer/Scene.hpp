#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include "Mesh.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "GlobInclude/WinInclude.hpp"

class Scene
{
public:
    Scene(Renderer &Renderer, ID3D12Device14 &Device);

    template <typename Vertex>
    std::shared_ptr<CPUMesh<Vertex>> MakeMesh(std::span<const Vertex> vertices, std::span<const uint16_t> indices)
    {
        auto MeshCPU = std::make_shared<CPUMesh<Vertex>>(vertices, indices);
        PendingMeshes.push_back(MeshCPU);
        return MeshCPU;
    }

   GameObject *CreateGameObject(std::shared_ptr<CPUMeshBase> CPUMesh);

   void FlushToRenderer(ID3D12GraphicsCommandList7 *CmdList);

private:
    Renderer &RendererRef;
    ID3D12Device14 &DeviceRef;
    std::vector<std::shared_ptr<CPUMeshBase>> PendingMeshes;
    std::vector<GameObject *> PendingObjects;
    std::vector<std::unique_ptr<GameObject>> OwnedObjects;
    std::unordered_map<std::shared_ptr<CPUMeshBase>, std::shared_ptr<GPUMesh>> MeshMap;
};
