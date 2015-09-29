/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ScreenCap
{
public:
    QWidget *centralWidget;
    QLineEdit *lineEditIp;
    QPushButton *pushButtonStart;
    QLabel *label;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ScreenCap)
    {
        if (ScreenCap->objectName().isEmpty())
            ScreenCap->setObjectName(QStringLiteral("ScreenCap"));
        ScreenCap->resize(300, 350);
        ScreenCap->setMinimumSize(QSize(300, 350));
        ScreenCap->setMaximumSize(QSize(300, 350));
        centralWidget = new QWidget(ScreenCap);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        lineEditIp = new QLineEdit(centralWidget);
        lineEditIp->setObjectName(QStringLiteral("lineEditIp"));
        lineEditIp->setGeometry(QRect(21, 21, 251, 31));
        QFont font;
        font.setFamily(QStringLiteral("Times New Roman"));
        font.setPointSize(16);
        lineEditIp->setFont(font);
        pushButtonStart = new QPushButton(centralWidget);
        pushButtonStart->setObjectName(QStringLiteral("pushButtonStart"));
        pushButtonStart->setGeometry(QRect(21, 70, 256, 256));
        pushButtonStart->setMinimumSize(QSize(256, 256));
        pushButtonStart->setMaximumSize(QSize(256, 256));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(16);
        pushButtonStart->setFont(font1);
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(23, 21, 251, 31));
        ScreenCap->setCentralWidget(centralWidget);
        label->raise();
        lineEditIp->raise();
        pushButtonStart->raise();
        statusBar = new QStatusBar(ScreenCap);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ScreenCap->setStatusBar(statusBar);

        retranslateUi(ScreenCap);

        QMetaObject::connectSlotsByName(ScreenCap);
    } // setupUi

    void retranslateUi(QMainWindow *ScreenCap)
    {
        ScreenCap->setWindowTitle(QApplication::translate("ScreenCap", "\345\275\225\345\261\217\344\274\240\350\276\223", 0));
        lineEditIp->setText(QApplication::translate("ScreenCap", "192.168.1.1", 0));
        pushButtonStart->setText(QString());
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class ScreenCap: public Ui_ScreenCap {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
