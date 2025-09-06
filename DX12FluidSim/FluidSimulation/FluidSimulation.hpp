#pragma once

class FluidSimulation
{
public:
    FluidSimulation(class Scene &Scene);
    ~FluidSimulation();

private:
    Scene &SceneRef;
};