#ifndef SRENDERER_H
#define SRENDERER_H

#include <QMainWindow>
#include <QPalette>
#include <QFileDialog>
#include <QColorDialog>
#include "SRendererWidget.h"
#include "About.h"

namespace Ui {
class SRenderer;
}

class SRenderer : public QMainWindow
{
    Q_OBJECT

public:
    enum Option{MUTITHREAD, FACECULLING};
    enum Style {DARK, LIGHT};
    explicit SRenderer(QWidget *parent = nullptr);
    ~SRenderer();
    void SetStyle(Style style);
    void SetOption(Option option, bool val);
    void SetLightColor(LightColorType type, QColor color);
    void SetCameraPara(CameraPara para, float val);
    void SetLightDir();
private Q_SLOTS:
    void on_actionLight_triggered();

    void on_actionDark_triggered();

    void on_actionopen_file_triggered();

    void on_actionsave_image_triggered();

    void on_LineCheckBox_clicked();

    void on_VertexCheckBox_clicked();

    void on_actionMultiThread_triggered();

    void on_actionFaceCulling_triggered();

    void on_FovSilder_valueChanged(int value);

    void on_NearSilder_valueChanged(int value);

    void on_SpecularColorSet_clicked();

    void on_DiffuseColorSet_clicked();

    void on_AmbientColorSet_clicked();

    void on_PitchSlider_valueChanged(int value);

    void on_YawDial_valueChanged(int value);

    void on_actionAbout_triggered();

private:
    Ui::SRenderer *ui;
    void InitUI();
    void InitSignalAndSlot();
    QColor specularColor;
    QColor diffuseColor;
    QColor ambientColor;
};

#endif // SRENDERER_H
