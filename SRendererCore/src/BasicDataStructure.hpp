#pragma once
#pragma warning(disable : 4251)

#include <QImage>
#include <QColor>
#include <QString>
#include <string>
#include <any>
#include <cassert>
#include <cmath>
#include <iostream>
#include <array>
#include "glm/glm.hpp"
#include "SRendererCoreExport.h"

using Color = glm::vec3;
using Vector2D = glm::vec2;
using Vector3D = glm::vec3;
using VectorI3D = glm::ivec3;
using Vector4D = glm::vec4;
using VectorI4D = glm::ivec4;
using Coord2D = glm::vec2;
using CoordI2D = glm::ivec2;
using Coord3D = glm::vec3;
using CoordI3D = glm::ivec3;
using Coord4D = glm::vec4;
using CoordI4D = glm::ivec4;
using BorderPlane = glm::vec4;
using BorderLine = glm::vec3;

enum RenderMode{FACE,EDGE,VERTEX};
enum RenderColorType{BACKGROUND, LINE, POINT};
enum LightColorType{DIFFUSE, SPECULAR, AMBIENT};

struct Vertex
{
    Coord3D worldSpacePos;
    union
    {
        Coord4D clipSpacePos;
        Coord4D ndcSpacePos;
    };
    CoordI2D screenPos;
    float screenDepth;
    Vector3D normal;
    Coord2D texCoord;
};

using Triangle = std::array<Vertex, 3>;
using Line = std::array<CoordI2D, 2>;

struct Fragment
{
    Coord3D worldSpacePos;
    CoordI2D screenPos;
    float screenDepth;
    Color fragmentColor;
    Vector3D normal;
    Coord2D texCoord;
};

struct Light
{
    union{
    Coord4D pos;
    Vector4D dir;
    };
    Color ambient;
    Color diffuse;
    Color specular;
};

struct Material
{
    int diffuse;
    int specular;
    float shininess;
};
