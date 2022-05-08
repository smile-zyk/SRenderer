#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTimer>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QImage>

#include "SRendererDevice.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QImage ConvertBufferToPixmap(FrameBuffer buffer);
protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    int w = 100;
    int h = 100;
    QTimer timer;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
