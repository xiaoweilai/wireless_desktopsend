#if 1
//#include <QtGui/QApplication>
#include <QApplication>
#include <QtGui>
#include "tcpclientfilesend.h"
#include <QTextCodec>
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::addLibraryPath("plugins");
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("gb18030"));
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    tcpClientFileSend w;
    w.show();
    
    return a.exec();
}

#else
#include <QApplication>
#include <QtCore>
#include "dialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr( QTextCodec::codecForName("gb2312"));
    Dialog dialog;
    dialog.show();
    return dialog.exec();
}

#endif
