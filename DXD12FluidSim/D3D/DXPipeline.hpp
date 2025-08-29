#pragma once

#include "GlobInclude/WinInclude.hpp"
#include <vector>

class DXContext;
class Window;

class DXPipeline
{
public:
    DXPipeline();
    ~DXPipeline();
    DXPipeline &operator=(const DXPipeline &) = delete;
    DXPipeline(const DXPipeline &) = delete;

public:
    void CreateGraphicsPipeline();

private:
    bool Init();
};