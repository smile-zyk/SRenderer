#include "SRenderer.h"
#include "ui_SRenderer.h"

static inline QString GenerateStyleSheet(QColor color)
{
    return "background-color: rgb("
            + QString::number(static_cast<int>(color.red())) + ','
            + QString::number(static_cast<int>(color.green())) + ','
            + QString::number(static_cast<int>(color.blue())) + ");";
}

SRenderer::SRenderer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SRenderer)
{
    ui->setupUi(this);
    InitUI();
    InitSignalAndSlot();
}

SRenderer::~SRenderer()
{
    delete ui;
}

void SRenderer::SetStyle(Style style)
{
    QFile f;
    if(style == LIGHT)
    {
        f.setFileName(":/qdarkstyle/light/style.qss");
        ui->RenderWidget->SetRenderColor({0.98f, 0.98f, 0.98f}, BACKGROUND);
        ui->RenderWidget->SetRenderColor({0.098f, 0.137f, 0.176f}, LINE);
        ui->RenderWidget->SetRenderColor({0.098f, 0.137f, 0.176f}, POINT);
        ui->actionLight->setChecked(true);
        ui->actionDark->setChecked(false);
    }
    else
    {
        f.setFileName(":/qdarkstyle/dark/style.qss");
        ui->RenderWidget->SetRenderColor({0.098f, 0.137f, 0.176f}, BACKGROUND);
        ui->RenderWidget->SetRenderColor({0.98f, 0.98f, 0.98f}, LINE);
        ui->RenderWidget->SetRenderColor({0.98f, 0.98f, 0.98f}, POINT);
        ui->actionDark->setChecked(true);
        ui->actionLight->setChecked(false);
    }
    f.open(QFile::ReadOnly | QFile::Text);
    QTextStream ts(&f);
    qApp->setStyleSheet(ts.readAll());
}

void SRenderer::SetOption(Option option, bool val)
{
    if(option == MUTITHREAD)
    {
        ui->actionMultiThread->setChecked(val);
        ui->RenderWidget->SetMultiThread(val);
    }
    else
    {
        ui->actionFaceCulling->setChecked(val);
        ui->RenderWidget->SetFaceCulling(val);
    }
}

void SRenderer::SetLightColor(LightColorType type, QColor color)
{
    switch(type)
    {
    case SPECULAR:
        ui->SpecularColor->setStyleSheet(GenerateStyleSheet(color));
        specularColor = color;
        break;
    case DIFFUSE:
        ui->DiffuseColor->setStyleSheet(GenerateStyleSheet(color));
        diffuseColor = color;
        break;
    case AMBIENT:
        ui->AmbientColor->setStyleSheet(GenerateStyleSheet(color));
        ambientColor = color;
        break;
    }
    ui->RenderWidget->SetLightColor({color.red() / 255.f,
                                     color.green() / 255.f,
                                     color.blue() / 255.f}, type);
}

void SRenderer::SetCameraPara(CameraPara para, float val)
{
    if(para == FOV)
    {
        ui->FovLabel->setText(QString::number(static_cast<int>(val)));
        ui->RenderWidget->camera.fov = val;
    }
    else
    {
        ui->NearLabel->setText(QString::number(val, 'f', 1));
        ui->RenderWidget->camera.zNear = val;
    }
}

void SRenderer::SetLightDir()
{
    Vector3D lightDir;
    float pitch = glm::radians(glm::clamp(static_cast<float>(ui->PitchSlider->value()), -89.9f, 89.9f));
    float yaw = -glm::radians(static_cast<float>(ui->YawDial->value()));

    lightDir.x = (1.f * (float)std::cos(pitch) * (float)std::sin(yaw));
    lightDir.y = (1.f * (float)std::sin(pitch));
    lightDir.z = (1.f * (float)std::cos(pitch) * (float)std::cos(yaw));

    ui->RenderWidget->SetLightDir(Vector4D(-lightDir, 0.f));
}

void SRenderer::on_actionLight_triggered()
{
    SetStyle(LIGHT);
}


void SRenderer::on_actionDark_triggered()
{
    SetStyle(DARK);
}

void SRenderer::on_actionopen_file_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Model File", "", "OBJ(*.obj)");
    if(!filePath.isEmpty())
        ui->RenderWidget->LoadModel(filePath);
}

void SRenderer::on_actionsave_image_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG(*.png)");
    if(!fileName.isEmpty())
        ui->RenderWidget->SaveImage(fileName);
}

void SRenderer::InitUI()
{
    setFixedSize(1120,672);
    ui->Setting->setTabText(0, "Light");
    ui->Setting->setTabText(1, "Model");
    SetStyle(LIGHT);
    SetOption(MUTITHREAD,true);
    SetOption(FACECULLING,true);
    SetCameraPara(FOV, 60.f);
    SetCameraPara(NEAR, 1.0f);
    SetLightColor(SPECULAR, QColor(255, 255, 255));
    SetLightColor(DIFFUSE, QColor(153, 153, 153));
    SetLightColor(AMBIENT, QColor(102, 102, 102));
    SetLightDir();
    QColorDialog::setCustomColor(0,QColor(255, 255, 255));
    QColorDialog::setCustomColor(2,QColor(153, 153, 153));
    QColorDialog::setCustomColor(4,QColor(102, 102, 102));
}

void SRenderer::InitSignalAndSlot()
{
    connect(ui->RenderWidget, &SRendererWidget::SendModelData,this,
            [this](int triangleCount, int vertexCount)
    {
        ui->TriangleNumberLabel->setText(QString::number(triangleCount));
        ui->VertexNumberLabel->setText(QString::number(vertexCount));
    });
}

void SRenderer::on_LineCheckBox_clicked()
{
    if(ui->LineCheckBox->isChecked())
    {
        if(ui->VertexCheckBox->isChecked())
            ui->VertexCheckBox->setChecked(false);
        ui->RenderWidget->SetRenderMode(EDGE);
    }
    else
    {
        ui->RenderWidget->SetRenderMode(FACE);
    }
}

void SRenderer::on_VertexCheckBox_clicked()
{
    if(ui->VertexCheckBox->isChecked())
    {
        if(ui->LineCheckBox->isChecked())
            ui->LineCheckBox->setChecked(false);
        ui->RenderWidget->SetRenderMode(VERTEX);
    }
    else
    {
        ui->RenderWidget->SetRenderMode(FACE);
    }
}

void SRenderer::on_actionMultiThread_triggered()
{
    SetOption(MUTITHREAD, ui->actionMultiThread->isChecked());
}


void SRenderer::on_actionFaceCulling_triggered()
{
    SetOption(FACECULLING, ui->actionFaceCulling->isChecked());
}


void SRenderer::on_FovSilder_valueChanged(int value)
{
    SetCameraPara(FOV, static_cast<float>(value));
}

void SRenderer::on_NearSilder_valueChanged(int value)
{
    SetCameraPara(NEAR, value / 10.f);
}

void SRenderer::on_SpecularColorSet_clicked()
{
    QColor color = QColorDialog::getColor(specularColor,this,"Select Specular Color");
    if(color.isValid())
        SetLightColor(SPECULAR, color);
}

void SRenderer::on_DiffuseColorSet_clicked()
{
    QColor color = QColorDialog::getColor(diffuseColor,this,"Select Diffuse Color");
    if(color.isValid())
        SetLightColor(DIFFUSE, color);
}

void SRenderer::on_AmbientColorSet_clicked()
{
    QColor color = QColorDialog::getColor(ambientColor,this,"Select Ambient Color");
    if(color.isValid())
        SetLightColor(AMBIENT, color);
}

void SRenderer::on_PitchSlider_valueChanged(int value)
{
    SetLightDir();
}

void SRenderer::on_YawDial_valueChanged(int value)
{
    SetLightDir();
}

void SRenderer::on_actionAbout_triggered()
{
    About about;
    about.exec();
}
