#include "About.h"
#include "ui_About.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    ui->textBrowser->setOpenLinks(true);
    ui->textBrowser->setOpenExternalLinks(true);
    setFixedSize(400, 220);
}

About::~About()
{
    delete ui;
}
