#pragma once
#include "GlobInclude/WinInclude.hpp"

struct Vertex
{
    float x, y, z;
    float r, g, b;
    float u, v;

    static D3D12_INPUT_LAYOUT_DESC GetInputLayout()
    {
        static D3D12_INPUT_ELEMENT_DESC layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            {"TEXCOORD",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
        };
        return {layout, _countof(layout)};
    }
};
