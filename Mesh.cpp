#include "Mesh.h"
#include <QDebug>
void Mesh::Draw()
{
    SRendererDevice::GetInstance().vertexList = vertices;
    SRendererDevice::GetInstance().indices = indices;
    SRendererDevice::GetInstance().shader->material.diffuse = diffuseTextureIndex;
    SRendererDevice::GetInstance().shader->material.specular = specularTextureIndex;
    SRendererDevice::GetInstance().Render();
}
