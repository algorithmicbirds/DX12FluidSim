#include "D3D/DXPipeline.hpp"
#include "Vertex.hpp"

DXPipeline::DXPipeline() {}

DXPipeline::~DXPipeline() {}

void DXPipeline::CreateGraphicsPipeline()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GraphicsPSODesc{};

    GraphicsPSODesc.InputLayout = Vertex::GetInputLayout();
    GraphicsPSODesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
}
