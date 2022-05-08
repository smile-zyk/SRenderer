#pragma once

#include "Shader.hpp"
#include "SRendererCoreExport.h"

class SRENDERERCORE_EXPORT TestShader : public Shader
{
public:
    virtual void VertexShader(Vertex &vertex) override;
    virtual void FragmentShader(Fragment &Fragment) override;
};
