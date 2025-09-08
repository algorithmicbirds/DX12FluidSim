#pragma once


class FluidSimulation
{
public:
    FluidSimulation(class Scene &Scene, class Renderer &Renderer);
    ~FluidSimulation();

private:
    Scene &SceneRef;
    Renderer &RendererRef;
};