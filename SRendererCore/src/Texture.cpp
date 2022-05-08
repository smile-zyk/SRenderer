#include "Texture.h"
#include <QDebug>

bool Texture::LoadFromImage(QString path)
{
    this->path = path;
    if(texture.load(path))
    {
        texture.mirror();
        w = texture.width();
        h = texture.height();
        return true;
    }
    return false;
}

Color Texture::Sample2D(Coord2D coord)
{
    int x = static_cast<int>(coord.x * w - 0.5f) % w;
    int y = static_cast<int>(coord.y * h - 0.5f) % h;
    x = x < 0 ? w + x : x;
    y = y < 0 ? h + y : y;
    return Color(texture.pixelColor(x, y).red() / 255.f, texture.pixelColor(x, y).green() / 255.f, texture.pixelColor(x, y).blue() / 255.f);
}
