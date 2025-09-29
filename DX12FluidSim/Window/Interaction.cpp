#include "Interaction.hpp"
#include <iostream>

Interaction::Interaction(Window &Wnd, ConstantBuffers &CB) : WindowRef(Wnd), CBRef(CB) {}

Interaction::~Interaction() {}

void Interaction::Update()
{
    bool bIsLeftMBDown = WindowRef.Mouse.LeftIsPressed() ? 1 : 0;
    bool bIsRighMBDown = WindowRef.Mouse.RightIsPressed() ? 1 : 0;
    if (bIsRighMBDown == 1)
    {
        CalculateMouseCoordForGPU();
        CBRef.OnMouseRBDown(bIsRighMBDown);
        CBRef.OnMouseMove({MouseX, MouseY});
        std::cout << "Right Is pressed x: " << MouseX << " y: " << MouseY << "\n";
    }
    else
    {
        CBRef.OnMouseRBDown(bIsRighMBDown);
    }
    if (bIsLeftMBDown == 1)
    {
        CalculateMouseCoordForGPU();
        CBRef.OnMouseLBDown(bIsLeftMBDown);
        CBRef.OnMouseMove({MouseX, MouseY});
        std::cout << "Left Is pressed x: " << MouseX << " y: " << MouseY << "\n";
    }
    else
    {
        CBRef.OnMouseLBDown(bIsLeftMBDown);
    }
}

void Interaction::CalculateMouseCoordForGPU()
{
    RECT ClientRect;
    GetClientRect(WindowRef.GetHwnd(), &ClientRect);
    int ClientWidth = ClientRect.right - ClientRect.left;
    int ClientHeight = ClientRect.bottom - ClientRect.top;
    
    MouseX = static_cast<float>(WindowRef.Mouse.GetPosX()) / static_cast<float>(ClientWidth);
    MouseY = static_cast<float>(WindowRef.Mouse.GetPosY()) / static_cast<float>(ClientHeight);
    MouseX = std::clamp(MouseX, 0.0f, 1.0f);
    MouseY = std::clamp(MouseY, 0.0f, 1.0f);
    MouseY = 1.0f - MouseY;
    if (!WindowRef.Mouse.IsInWindow())
    {
        MouseX = MouseY = 0;
    }
}
