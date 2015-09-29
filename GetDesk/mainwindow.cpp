#include "mainwindow.h"
#include "ui_mainwindow.h"

ScreenCap::ScreenCap(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenCap)
{
    ui->setupUi(this);
}

ScreenCap::~ScreenCap()
{
    delete ui;
}
