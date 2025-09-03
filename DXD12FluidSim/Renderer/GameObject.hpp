#pragma once

#include "GlobInclude/WinInclude.hpp"
#include "Renderer/Mesh.hpp"
#include <memory>
#include "D3D/Vertex.hpp"

struct Transform
{
    DirectX::XMFLOAT3 Translation{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 Rotation{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 Scale{1.0f, 1.0f, 1.0f};

    DirectX::XMFLOAT4X4 ModelMatrix;

    void UpdateMatrix()
    {
        using namespace DirectX;
        XMMATRIX T = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
        XMMATRIX R = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
        XMMATRIX S = XMMatrixScaling(Scale.x, Scale.y, Scale.z);

        XMMATRIX M = S * R * T;
        XMStoreFloat4x4(&ModelMatrix, M);
    }

    Transform() { UpdateMatrix(); }
};

class GameObject
{
public:
    typedef unsigned int id_t;

public:
    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;

    static GameObject CreateGameObject()
    {
        static id_t CurrentID = 0;
        return GameObject(CurrentID++);
    }

public:
    Transform Transform;

    // Currently bound to Mesh<Vertex>.
    // Generalize to support multiple vertex types (via templates or a common base) later.
    std::shared_ptr<Mesh<Vertex>> Mesh;


private:
    GameObject(id_t ObjectId) : ID{ObjectId} {};
    id_t ID;
};
