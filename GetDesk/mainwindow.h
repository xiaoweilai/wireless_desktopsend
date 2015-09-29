#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class ScreenCap;
}

class ScreenCap : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenCap(QWidget *parent = 0);
    ~ScreenCap();

private:
    Ui::ScreenCap *ui;
};

#endif // MAINWINDOW_H
