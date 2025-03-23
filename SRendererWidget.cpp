#include "SRendererWidget.h"
#include "ui_SRendererWidget.h"
#include <QDebug>
#include <chrono>

int lastFrameTime;
int deltaTime;
QPoint lastPos;
QPoint currentPos;
int ratio;
Qt::MouseButtons currentBtns;
glm::mat4 ModelMatrix;

SRendererWidget::SRendererWidget(QWidget *parent) :
    QWidget(parent),camera((float)DEFAULT_WIDTH/DEFAULT_HEIGHT, FIXED_CAMERA_FAR),
    w(DEFAULT_WIDTH),h(DEFAULT_HEIGHT),
    ui(new Ui::SRendererWidget),model(nullptr)
{
    ui->setupUi(this);
    setFixedSize(w, h);
    ui->FPSLabel->setStyleSheet("background:transparent");
    ui->FPSLabel->setVisible(false);
    InitDevice();
    // set render frequency
    connect(&timer,&QTimer::timeout,this,&SRendererWidget::Render);
    timer.start(1);
}

SRendererWidget::~SRendererWidget()
{
    delete ui;
    delete model;
}

void SRendererWidget::ResetCamera()
{
    ui->FPSLabel->setVisible(true);
    camera.SetModel(model->centre,model->GetYRange());
    ModelMatrix = glm::mat4(1.0f);
}

void SRendererWidget::SetRenderColor(Color color, RenderColorType type)
{
    switch(type)
    {
    case BACKGROUND:
        SRendererDevice::GetInstance().clearColor = color;
        break;
    case LINE:
        SRendererDevice::GetInstance().lineColor = color;
        break;
    case POINT:
        SRendererDevice::GetInstance().pointColor = color;
    }
}

void SRendererWidget::SetLightColor(Color color, LightColorType type)
{
    switch(type)
    {
    case DIFFUSE:
        SRendererDevice::GetInstance().shader->lightList[0].diffuse = color;
        break;
    case SPECULAR:
        SRendererDevice::GetInstance().shader->lightList[0].specular = color;
        break;
    case AMBIENT:
        SRendererDevice::GetInstance().shader->lightList[0].ambient = color;
        break;
    }
}

void SRendererWidget::LoadModel(QString path)
{
    Model *newModel = new Model(path);
    if(!newModel->loadSuccess)
    {
        QMessageBox::critical(this,"Error","Model loading error!");
        delete newModel;
        return;
    }
    if(model != nullptr)
        delete model;
    model = newModel;
    Q_EMIT SendModelData(model->triangleCount,model->vertexCount);
    ResetCamera();
}

void SRendererWidget::InitDevice()
{
    SRendererDevice::Init(w,h);
    SRendererDevice::GetInstance().shader = std::make_unique<BlinnPhongShader>();
    SRendererDevice::GetInstance().shader->lightList.push_back(Light());
}

void SRendererWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, SRendererDevice::GetInstance().GetBuffer());
}

void SRendererWidget::mousePressEvent(QMouseEvent *event)
{
    currentBtns = event->buttons();
    currentPos = event->pos();
    lastPos = {0,0};
}

void SRendererWidget::mouseReleaseEvent(QMouseEvent *event)
{
    currentBtns = event->buttons();
}

void SRendererWidget::mouseMoveEvent(QMouseEvent *event)
{
    currentPos = event->pos();
}

void SRendererWidget::wheelEvent(QWheelEvent *event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    QPoint res;
    if (!numPixels.isNull()) {
        res = numPixels;
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        res = numSteps;
    }
    ratio += res.y();
}

void SRendererWidget::ProcessInput()
{
    if((currentBtns & Qt::LeftButton) || (currentBtns & Qt::RightButton))
    {
        if(!lastPos.isNull())
        {
            Vector2D motion = {(float)(currentPos - lastPos).x(),(float)(currentPos - lastPos).y()};
            motion.x = (motion.x / w);
            motion.y = (motion.y / h);
            if(currentBtns & Qt::LeftButton)
            {
                camera.RotateAroundTarget(motion);
            }
            if(currentBtns & Qt::RightButton)
            {
                camera.MoveTarget(motion);
            }
        }
        lastPos = currentPos;
    }
    if(ratio != 0)
    {
        camera.CloseToTarget(ratio);
        ratio = 0;
    }
}

void SRendererWidget::Render()
{
    SRendererDevice::GetInstance().ClearBuffer();
    if(model == nullptr) return;
    int nowTime = QTime::currentTime().msecsSinceStartOfDay();
    if(lastFrameTime != 0)
    {
        deltaTime = nowTime - lastFrameTime;
        ui->FPSLabel->setText(QStringLiteral("FPS : ")+QString::number(1000.0 / deltaTime, 'f', 0));
    }
    lastFrameTime = nowTime;
    ProcessInput();
    SRendererDevice::GetInstance().shader->Model = ModelMatrix;
    SRendererDevice::GetInstance().shader->View = camera.GetViewMatrix();
    SRendererDevice::GetInstance().shader->Projection = camera.GetProjectionMatrix();
    SRendererDevice::GetInstance().shader->eyePos = camera.position;
    SRendererDevice::GetInstance().shader->material.shininess = 150.f;
//    auto start = std::chrono::system_clock::now();
    model->Draw();
//    auto finish = std::chrono::system_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
//    auto cost = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
//    qDebug() << cost;
    update();
}
