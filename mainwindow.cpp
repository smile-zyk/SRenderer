#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile f(":/qdarkstyle/dark/style.qss");

    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
    setFixedSize(820,840);
    SRendererDevice::InitDevice(w,h);
    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::ConvertBufferToPixmap(FrameBuffer buffer)
{
    QImage img(w,h,QImage::Format_RGB888);
    for(int y = 0; y < h ; y++)
        for(int x = 0; x < w; x++)
        {
            Color color = buffer.GetPixel(x,y);
            color = 255.f * color;
            img.setPixelColor(x,y,QColor(color.x,color.y,color.z));
        }
    return img;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(ui->centralwidget);
    SRendererDevice::GetDevice()->ClearBuffer(Color(1.f,1.f,0.f));
    QImage image = ConvertBufferToPixmap(SRendererDevice::GetDevice()->GetBuffer());
    painter.drawImage(10,10,image);
    painter.drawRect(10,10,50,50);
}

