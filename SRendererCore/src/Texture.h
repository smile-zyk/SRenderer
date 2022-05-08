#ifndef TEXTURE_H
#define TEXTURE_H
#include <QString>
#include <QImage>
#include "BasicDataStructure.hpp"
#include "SRendererCoreExport.h"

class SRENDERERCORE_EXPORT Texture
{
    enum
    {
        DIFFUSE,
        SPECLUAR
    };
    int w;
    int h;
    QImage texture;
public:
    QString path;
    Texture() = default;
    bool LoadFromImage(QString path);
    Color Sample2D(Coord2D coord);
};

#endif // TEXTURE_H
