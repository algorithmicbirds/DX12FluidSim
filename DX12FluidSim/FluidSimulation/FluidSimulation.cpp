#include "FluidSimulation.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Scene.hpp"

FluidSimulation::FluidSimulation(Scene &Scene) : SceneRef(Scene){
    Vertex CubeVertices[8] = {
        {-0.5f, -0.5f, -0.5f, 1,    0,    0   },
        {0.5f,  -0.5f, -0.5f, 0,    1,    0   },
        {0.5f,  0.5f,  -0.5f, 0,    0,    1   },
        {-0.5f, 0.5f,  -0.5f, 1,    1,    0   },
        {-0.5f, -0.5f, 0.5f,  0,    1,    1   },
        {0.5f,  -0.5f, 0.5f,  1,    0,    1   },
        {0.5f,  0.5f,  0.5f,  1,    1,    1   },
        {-0.5f, 0.5f,  0.5f,  0.5f, 0.5f, 0.5f}
    };

    uint16_t CubeIndices[36] = {4, 5, 6, 4, 6, 7, 0, 2, 1, 0, 3, 2, 0, 7, 3, 0, 4, 7,
                                1, 2, 6, 1, 6, 5, 3, 7, 6, 3, 6, 2, 0, 1, 5, 0, 5, 4};
    auto CubeMesh = SceneRef.MakeMesh<Vertex>(CubeVertices, CubeIndices);
    auto CubeObj = SceneRef.CreateGameObject(CubeMesh);
    CubeObj->Transform.Rotation = {1.0f, 1.0f, 0.0f};
}

FluidSimulation::~FluidSimulation() {}
