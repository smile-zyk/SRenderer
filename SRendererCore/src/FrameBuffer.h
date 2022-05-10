#pragma once

#include "BasicDataStructure.hpp"
#include "SRendererCoreExport.h"
#include <tbb/concurrent_vector.h>
#include <string>
#include <cfloat>
#include <QImage>
#include <QColor>
#include <QString>

class FrameBuffer
{
public:
    FrameBuffer(int _w, int _h);
    bool JudgeDepth(int x, int y, float z);
    void SetPixel(int x, int y, Color color);
    bool SaveImage(QString filePath);
    void ClearBuffer(Color color);
    QImage& GetImage(){return colorBuffer;}
private:
	int w;
	int h;
    std::vector<float> depthBuffer;
    QImage colorBuffer;
};
