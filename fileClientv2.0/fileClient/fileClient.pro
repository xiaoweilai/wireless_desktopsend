#-------------------------------------------------
#
# Project created by QtCreator 2014-12-21T23:13:09
#
#-------------------------------------------------

QT       += core gui network
QT       += widgets

TARGET = fileClient
TEMPLATE = app


SOURCES += main.cpp\
        tcpclientfilesend.cpp

HEADERS  += tcpclientfilesend.h

FORMS    += tcpclientfilesend.ui

RC_FILE = logo.rc

OTHER_FILES += \
    images/start.png \
    images/stop.png

RESOURCES += \
    gSource.qrc
