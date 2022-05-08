#include "SRenderer.h"
#define GLM_FORCE_AVX2
#include <QApplication>

int main(int argc, char *argv[])
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
    QApplication a(argc, argv);
    SRenderer w;
    w.show();
    return a.exec();
}
