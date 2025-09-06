#include "Scene.hpp"
#include "Renderer/Renderer.hpp"

Scene::Scene(Renderer &Renderer, ID3D12Device14 &Device) : RendererRef(Renderer), DeviceRef(Device) {}

GameObject *Scene::CreateGameObject(std::shared_ptr<CPUMeshBase> CPUMesh)
{
    std::unique_ptr<GameObject> GameObj = std::make_unique<GameObject>(GameObject::CreateGameObject());
    GameObj->CPUMesh = CPUMesh;
    GameObj->GPUMesh = nullptr;

    GameObject *GameObjPtr = GameObj.get();
    OwnedObjects.push_back(std::move(GameObj));
    PendingObjects.push_back(GameObjPtr);
    return GameObjPtr;
}

void Scene::FlushToRenderer(ID3D12GraphicsCommandList7 *CmdList)
{
    for (std::shared_ptr<CPUMeshBase> CPUMeshes : PendingMeshes)
    {
        std::shared_ptr<GPUMesh> MeshGPU = std::make_shared<GPUMesh>();
        if (CPUMeshes->UploadFunc)
        {
            CPUMeshes->UploadFunc(MeshGPU.get(), DeviceRef, CmdList);
            
        }
        MeshMap[CPUMeshes] = MeshGPU;
    }

    PendingMeshes.clear();
    for (GameObject *GameObj : PendingObjects)
    {
        GameObj->GPUMesh = MeshMap[GameObj->CPUMesh];
        RendererRef.RegisterGameObject(GameObj, CmdList);
    }
    PendingObjects.clear();
}
