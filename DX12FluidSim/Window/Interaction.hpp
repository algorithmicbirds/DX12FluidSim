#pragma once

#include "Window/Window.hpp"
#include "Renderer/ConstantBuffers.hpp"

class Interaction
{
public:
    Interaction(Window &Wnd, ConstantBuffers &CB);
    ~Interaction();
    Interaction(const Interaction &) = delete;
    Interaction &operator=(const Interaction &) = delete;

public:
    void Update();

private:
    void CalculateMouseCoordForGPU();

private:
    Window &WindowRef;
    ConstantBuffers &CBRef;

    float MouseX;
    float MouseY;
    float BBHeight = SimInitials::BoundingBoxHeight;
    float BBWidth = SimInitials::BoundingBoxWidth;
};