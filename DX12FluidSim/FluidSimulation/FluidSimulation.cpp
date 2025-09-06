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

    Vertex CubeVertices[8] = {
        {-0.5f, -0.5f, -0.5f, 1, 0, 0, 0.0f, 1.0f},
        {0.5f,  -0.5f, -0.5f, 0, 1, 0, 1.0f, 1.0f},
        {0.5f,  0.5f,  -0.5f, 0, 0, 1, 1.0f, 0.0f}, 
        {-0.5f, 0.5f,  -0.5f, 1, 1, 0, 0.0f, 0.0f}, 
        {-0.5f, -0.5f, 0.5f,  1, 0, 1, 0.0f, 1.0f}, 
        {0.5f,  -0.5f, 0.5f,  0, 1, 1, 1.0f, 1.0f}, 
        {0.5f,  0.5f,  0.5f,  1, 1, 1, 1.0f, 0.0f}, 
        {-0.5f, 0.5f,  0.5f,  0, 0, 0, 0.0f, 0.0f}  
    };
    uint16_t CubeIndices[36] = {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 4, 5, 1, 4, 1, 0,
                                3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7};
    Vertex PyramidVertices[5] = {
        {-0.5f, -0.5f, -0.5f, 1, 0, 0, 0.0f, 1.0f}, 
        {0.5f,  -0.5f, -0.5f, 0, 1, 0, 1.0f, 1.0f}, 
        {0.5f,  -0.5f, 0.5f,  0, 0, 1, 1.0f, 0.0f}, 
        {-0.5f, -0.5f, 0.5f,  1, 1, 0, 0.0f, 0.0f}, 
        {0.0f,  0.5f,  0.0f,  1, 0, 1, 0.5f, 0.5f}  
    };
    uint16_t PyramidIndices[18] = {0, 1, 2, 0, 2, 3, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};
    Vertex TriVertices[3] = {
        {0.0f,  0.5f,  0.0f, 1, 0, 0, 0.5f, 0.0f},
        {0.5f,  -0.5f, 0.0f, 0, 1, 0, 1.0f, 1.0f},
        {-0.5f, -0.5f, 0.0f, 0, 0, 1, 0.0f, 1.0f}
    };
    uint16_t TriIndices[3] = {0, 1, 2};

    auto CircleObj = SceneRef.CreateGameObject(SceneRef.MakeMesh<Vertex>(CircleVertices, CircleIndices));
    auto TriObj = SceneRef.CreateGameObject(SceneRef.MakeMesh<Vertex>(TriVertices, TriIndices));
    auto CubeObj = SceneRef.CreateGameObject(SceneRef.MakeMesh<Vertex>(CubeVertices, CubeIndices));
    auto PyramidObj = SceneRef.CreateGameObject(SceneRef.MakeMesh<Vertex>(PyramidVertices, PyramidIndices));

    CircleObj->Pipeline = RendererRef.GetCirclePipeline();
    TriObj->Pipeline = RendererRef.GetMeshPipeline();
    CubeObj->Pipeline = RendererRef.GetMeshPipeline();
    PyramidObj->Pipeline = RendererRef.GetMeshPipeline();
    CircleObj->Transform.Translation = {-2.0f, 1.5f, 0.0f};  
    TriObj->Transform.Translation = {2.0f, 1.5f, 0.0f};      
    CubeObj->Transform.Translation = {-2.0f, -1.2f, 0.0f};   
    PyramidObj->Transform.Translation = {2.0f, -1.2f, 0.0f}; 

    CircleObj->Transform.Scale = {0.8f, 0.8f, 1.0f};
    TriObj->Transform.Scale = {0.8f, 0.8f, 1.0f};
    CubeObj->Transform.Scale = {0.8f, 0.8f, 0.8f};
    PyramidObj->Transform.Scale = {0.8f, 0.8f, 0.8f};
}

FluidSimulation::~FluidSimulation() {}
