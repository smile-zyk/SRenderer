#ifndef SRENDERERWIDGET_H
#define SRENDERERWIDGET_H

#include <QWidget>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QFile>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QTime>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMessageBox>
#include "SRendererDevice.h"
#include "BlinnPhongShader.h"
#include "Model.h"
#include "Camera.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define FIXED_CAMERA_FAR 100.f

namespace Ui {
class SRendererWidget;
}

class SRendererWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SRendererWidget(QWidget *parent = nullptr);
    ~SRendererWidget();
    void SetRenderColor(Color color, RenderColorType type);
    void SetLightColor(Color color, LightColorType type);
    void SetLightDir(Vector4D dir){SRendererDevice::GetInstance().shader->lightList[0].dir = dir;}
    void SetRenderMode(RenderMode mode){SRendererDevice::GetInstance().renderMode = mode;}
    void SetFaceCulling(bool val){SRendererDevice::GetInstance().faceCulling = val;}
    void SetMultiThread(bool val){SRendererDevice::GetInstance().multiThread = val;}
    void SaveImage(QString path){SRendererDevice::GetInstance().SaveImage(path);}
    void LoadModel(QString path);
    void InitDevice();
    Camera camera;
protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
Q_SIGNALS:
    void SendModelData(int triangleCount, int vertexCount);
public Q_SLOTS:
    void Render();

private:
    int w;
    int h;
    QTimer timer;
    void ProcessInput();
    void ResetCamera();
    void InitUI();
    Ui::SRendererWidget *ui;
    Model* model;
};

#endif // SRENDERERWIDGET_H
