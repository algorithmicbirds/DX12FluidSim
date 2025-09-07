#include "FluidSimulation.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Scene.hpp"

FluidSimulation::FluidSimulation(Scene &Scene, Renderer &Renderer) : SceneRef(Scene), RendererRef(Renderer)
{
    Vertex CircleVertices[4] = {
        {-0.5f, -0.5f, 0.0f, 1, 0, 0, 0.0f, 1.0f},
        {0.5f,  -0.5f, 0.0f, 0, 1, 0, 1.0f, 1.0f},
        {0.5f,  0.5f,  0.0f, 1, 0, 1, 1.0f, 0.0f},
        {-0.5f, 0.5f,  0.0f, 0, 1, 0, 0.0f, 0.0f}
    };

    uint16_t CircleIndices[6] = {0, 1, 2, 0, 2, 3};

    auto CircleObj = SceneRef.CreateGameObject(SceneRef.MakeMesh<Vertex>(CircleVertices, CircleIndices));
    
    CircleObj->Pipeline = RendererRef.GetCirclePipeline();
}

FluidSimulation::~FluidSimulation() {}
