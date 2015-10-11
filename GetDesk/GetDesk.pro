#-------------------------------------------------
#
# Project created by QtCreator 2015-09-29T08:15:50
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GetDesk
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    version.h

FORMS    += mainwindow.ui

RC_FILE = logo.rc

RESOURCES += \
    gSource.qrc
