#include "tcpclientfilesend.h"
#include "ui_tcpclientfilesend.h"
#include <QTextCodec>
#include <QPixmap>
#include <QDesktopWidget>
#include <windows.h>
#include <QIODevice>
#include <QMessageBox>
#include <QScreen>
#include <QDesktopWidget>

//#define DEBUG    /* 调试信息 */
//#define TIMETEST /* 耗时测试 */

const char version_filetransClient[]="v2.0";


#if 1
//编码汉字
//#define str_china(A)     QString::fromLocal8Bit(#A)
#define str_china(A)     QString::fromLocal8Bit(A)
//#define str_china(A)     QString::fromUtf8(#A)
#else
#define str_china(A)     codec->toUnicode(#A)
#endif

#if 1
#define DEFAULT_HOSTADDR "192.168.1.104"
//#define DEFAULT_HOSTADDR "169.254.194.157"
#else
#define DEFAULT_HOSTADDR "127.0.0.1"
#endif
#define DEFAULT_PORT   "16689"

#define OVERIMAGENUMS (500)

#define IMGPOOLSIZE 3 /* 图片池塘大小 */

tcpClientFileSend::tcpClientFileSend(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tcpClientFileSend),
    pscreen(NULL)
{

    ui->setupUi(this);
    ui->lineEditHost->setStyleSheet("QLineEdit{font: bold italic large \"Times New Roman\";font-size:25px;color:rgb(55,100,255);height:50px;border:4px solid rgb(155,200,33);background-color: rgba(0,0,0,30);border-radius:15px;selection-color:pink}");
    ui->label->setStyleSheet("QLabel{font: bold italic large \"Times New Roman\";font-size:14px;color:rgb(55,100,255);height:50px;selection-color:pink}");
    //    ui->label->setStyleSheet("QLabel{font: bold italic large \"Times New Roman\";font-size:18px;color:rgb(55,100,255);height:50px;border:4px solid rgb(155,200,33);background-color: rgba(0,0,0,30);border-radius:15px;selection-color:pink}");
    showVerion();
    connect(ui->lineEditHost,SIGNAL(textChanged(QString)),this,SLOT(textCheck(QString)));

    pscreen = QGuiApplication::primaryScreen();;
    picNametime = 1;
    TotalBytes = 0;
    byteWritten = 0;

    jpgnameNo = 1; //图片名称计数
    time_total = 0.0;//耗时时间
    namelst.clear();//保存的文件名列表
    sizelst.clear();//保存的文件大小列表
    sendDoneFlag = SEND_DONE;//发送结束标志
    emitSigNums = 0;//发送信号的次数
    curstate = STATE_PAUSE;//当前状态，开启或暂停
    p_tcpClient = NULL;//tcp socket
    imgVecArray.clear();
    imgLstArray.clear();

    //    ui->openButton->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->lineEditHost->setText(ReadIpAddr());

    connect(ui->startButton,SIGNAL(clicked()),this,SLOT(start()));
    //    connect(ui->pauseButton,SIGNAL(clicked()),this,SLOT(pause()));
    //    connect(ui->openButton,SIGNAL(clicked()),this,SLOT(opsenFile()));
    connect(ui->disconnectButton,SIGNAL(clicked()),this,
            SLOT(disconnectSocket()));
    connect(qApp,SIGNAL(lastWindowClosed()),this,
            SLOT(disconnectSocket()));

    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,
            SLOT(startTransfer()));

    connect(this,SIGNAL(emitImgZeroSignal()),this,
            SLOT(parseImage()));

    connect(qApp,SIGNAL(lastWindowClosed()),this,SLOT(ShutDownAll()));

}

tcpClientFileSend::~tcpClientFileSend()
{
    delete ui;
}
//连接
void tcpClientFileSend::openFile()
{
    ui->startButton->setEnabled(true);
    ui->clientStatusLabel->setText(str_china("请点击开始远程传输"));
    //创建tcpsocket
    p_tcpClient = new QTcpSocket;
    if(!p_tcpClient)
        return;

    connect(p_tcpClient,SIGNAL(connected()),this,
            SLOT(startTransfer()));
    connect(p_tcpClient,SIGNAL(bytesWritten(qint64)),this,
            SLOT(updateClientProgress(qint64)));
    connect(p_tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayErr(QAbstractSocket::SocketError)));
    //    ui->openButton->setEnabled(false);

}


void tcpClientFileSend::start()
{
    openFile();
#ifdef SHOWCURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    byteWritten = 0;
    curstate = STATE_START;
    ui->startButton->setEnabled(false);
    //    ui->pauseButton->setEnabled(true);


    ui->clientStatusLabel->setText(str_china("连接中..."));
    p_tcpClient->connectToHost(ui->lineEditHost->text(),
                               QString(DEFAULT_PORT).toInt());
    p_tcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 1);//优化为最低延迟，后面的1代码启用该优化。


    //waitForConnected()等待连接知道超过最大等待时间。如果连接建立函数返回true；否则返回false。
    //当返回false时可以调用error来确定无法连接的原因
    if(!p_tcpClient->waitForConnected(3000))
    {
        qDebug() <<"here:" << p_tcpClient;
        if(NULL != p_tcpClient)
        {
            qDebug() <<"Error: "<<p_tcpClient->errorString();
            p_tcpClient->deleteLater();
            p_tcpClient = NULL;

            QMessageBox::information(this,str_china("网络"),
                                     str_china("产生如下错误：连接失败"));
        }
        curstate = STATE_PAUSE;
        timer->stop();

        //        ui->openButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        //        ui->pauseButton->setEnabled(false);
        ui->disconnectButton->setEnabled(false);
        ui->clientStatusLabel->setText(str_china("连接失败，请确认网络IP连接"));
    }else{
        if(!timer->isActive())
        {

            //            ui->openButton->setEnabled(false);
            ui->startButton->setEnabled(false);
            //            ui->pauseButton->setEnabled(true);
            ui->disconnectButton->setEnabled(true);
            curstate = STATE_START;
            timer->start(10);
        }
        SaveIpAddr(ui->lineEditHost->text());
        ui->clientStatusLabel->setText(str_china("连接成功"));
    }

}
//暂停
void tcpClientFileSend::pause()
{
    curstate = STATE_PAUSE;
    ui->startButton->setEnabled(true);
    //    ui->pauseButton->setEnabled(false);
    ui->disconnectButton->setEnabled(true);
    ui->clientStatusLabel->setText(str_china("暂停中..."));
}

/* 一旦连接建立成功，QTcpSocket类将发出connected消息，继而调用
startTransfer()槽函数。
 */
void tcpClientFileSend::startTransfer()
{
    if(STATE_PAUSE == curstate)
    {
        timer->stop();
        return;
    }

#ifdef TIMETEST /* 耗时测试 */
    QTime time;
    time.start(); //开始计时，以ms为单位

#endif /* 耗时测试 */

    //    ui->clientStatusLabel->setText(str_china("连接成功"));

    if(namelst.count() > IMGPOOLSIZE)
    {
        //        if(socketError == QTcpSocket::RemoteHostClosedError)
        //        {

        //        }
        return;
    }
    emitSigNums = 0;//发送信号归零

    fileImage = grabframeGeometry();
    //    if(ui->checkBox_speed->isChecked())
    //    {
    //        Sleep(200);
    //    }else
    //    {
    ////        Sleep(10);
    //    }

    //    fileImage =fileImage.convertToFormat(QImage::Format_Indexed8,Qt::AutoColor);

    QString fileName = QString("%1/%2.jpg").arg(dirname)
            .arg(jpgnameNo++);

#ifdef DEBUG
    qDebug() << "filename:" << fileName;
#endif

//    namelst.append(fileName);
    namelst.append("a.jpg");

    QByteArray bytearry;
    QBuffer buffer;
    buffer.setBuffer(&bytearry);
    fileImage.save(&buffer,STREAM_PIC_FORT);
    //    if(ui->radioButton_smooth->isChecked())
    //    {
    //        fileImage.save(&buffer,STREAM_PIC_FORT);
    //    }
    //    else if(ui->radioButton_highpix->isChecked())
    //    {
    //        QImage img;
    //        img = fileImage.convertToFormat(QImage::Format_ARGB32,Qt::AutoColor);
    //        img.save(&buffer,STREAM_PIC_FORT);
    //    }
    //    else
    //    {
    //        fileImage.save(&buffer,STREAM_PIC_FORT);
    //    }
    buffer.data();
    imgVecArray.append(bytearry);

#ifdef TIMETEST /* 耗时测试 */

    int time_Diff = time.elapsed(); //返回从上次start()或restart()开始以来的时间差，单位ms
    //以下方法是将ms转为s
    float f = time_Diff / 1000.0;
    time_total += f;

    qDebug() << "save  elaspe:" <<time_Diff <<"ms";
    qDebug() << "current time:" <<time_total <<"s";

#endif /* 耗时测试 */


    //    emit emitImgZeroSignal();
    parseImage();

    return;
}

void tcpClientFileSend::parseImage()
{

    if(namelst.count() == 0)
    {

#ifdef DEBUG
        qDebug() << "namelst count is 0!!,return";
        qDebug() << "======>>>network state:" << p_tcpClient->state();
#endif

        return;
    }

#ifdef DEBUG
    qDebug() << "namelst count:" << namelst.count();
#endif
    if(SEND_ING == sendDoneFlag)
        return;

    sendDoneFlag = SEND_ING;//发送中

    outBlock.resize(0);
    QString readFname = namelst.at(0);
#ifdef DEBUG
    qDebug() << "read file name:" << readFname;
#endif
    if(namelst.count() > 0)
        namelst.removeAt(0);
#ifdef DEBUG
    qDebug() << "after delete namelst count:" << namelst.count();
#endif

    //    QDir dir(QDir::currentPath());
    //    if(!dir.exists(dirname))
    //    {
    //        dir.mkdir(dirname);
    //    }

    //    QFile file(readFname);
    //    if(!file.open(QIODevice::ReadOnly)) {
    //#ifdef DEBUG
    //        qDebug()<<"Can't open the file!"<<readFname;
    //#endif
    //        return;
    //    }

    /*---------------------------------------
发送文件数据格式 ：

1.总长度 (8bytes) -- 总字节(文件大小 + 8字节 + 文件名)
2.文件名长度(qint64() 8Bytes)
3.文件名
---------------------------------------*/


    //    outBlockFile = file.readAll();
    outBlockFile = imgVecArray.at(0);
    imgVecArray.remove(0);
#ifdef DEBUG
    qDebug() <<"read file size:" << outBlockFile.size() ;
#endif

    TotalBytes = outBlockFile.size();



    outBlock.resize(0);
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.resetStatus();
    sendOut.setVersion(QDataStream::Qt_4_0);

    //发送文件名称
    sendOut <<qint64(0) <<qint64(0) <<readFname;
    //TotalBytes为总数据长度，即（数据量长度+文件名长度+文件名）
    TotalBytes += outBlock.size(); //加上图片名称长度
    sendOut.device()->seek(0);

    //总字节(文件大小 + 8字节 + 文件名) ，
    sendOut << TotalBytes << qint64((outBlock.size() - sizeof(qint64)*2));

    bytesToWrite = TotalBytes - p_tcpClient->write(outBlock);//将名称发出后，剩余图片大小
    //    ui->clientStatusLabel->setText(str_china("传输中..."));
    showTextTransfering();
#ifdef DEBUG
    qDebug() << "TotalBytes:" << TotalBytes;
#endif

    //    file.close();
    //    dir.remove(readFname);//删除文件


    return;

}

void tcpClientFileSend::updateClientProgress(qint64 numBytes)
{

#ifdef DEBUG
    qDebug() << "numBytes:--------->>"<<numBytes;
#endif
    byteWritten += (int)numBytes;
    if(bytesToWrite > 0)
    {
#ifdef DEBUG
        qDebug() <<"-->:outBlockFile size:" << outBlockFile.size();
#endif

        bytesToWrite -= (int)p_tcpClient->write(outBlockFile);


#ifdef DEBUG
        qDebug() <<"-->:bytesToWrite size:" << bytesToWrite;
#endif
    }
    else
    {
#ifdef DEBUG
        qDebug() << "-->: send image done!!";
#endif
        picNametime++;
        TotalBytes = 0;
        byteWritten = 0;
        sendDoneFlag = SEND_DONE;
        if(STATE_PAUSE == curstate)
        {
            timer->stop();
            ui->clientStatusLabel->setText(str_china("暂停中..."));
            return;
        }else{
            ui->clientStatusLabel->setText(str_china("传输中..."));
        }
        parseImage();
    }



}

void tcpClientFileSend::displayErr(QAbstractSocket::SocketError socketError)
{
    qDebug() << "display err";
    if(NULL == p_tcpClient)
    {
        QMessageBox::information(this,str_china("网络"),
                                 str_china("产生如下错误：连接失败"));
    }else{
        QMessageBox::information(this,str_china("网络"),
                                 str_china("产生如下错误： %1")
                                 .arg(p_tcpClient->errorString()));
    }

    disconnectSocket();


#ifdef SHOWCURSOR
    QApplication::restoreOverrideCursor();
#endif
}

//桌面尺寸
QImage tcpClientFileSend::grabDeskScreen()
{
    if (pscreen){
        return pscreen->grabWindow(0).toImage();
    }
    else
    {
        QImage img;
        qDebug() << "grab DeskScreen Err!";
        return img;
    }
}

//界面尺寸
QImage tcpClientFileSend::grabframeGeometry()
{
    return grabDeskScreen();
}

void tcpClientFileSend::deleteImgs()
{
    imgVecArray.clear();
    imgLstArray.clear();
}

void tcpClientFileSend::ShutDownAll()
{
    disconnectSocket();
    close();
}

//断开连接
void tcpClientFileSend::disconnectSocket()
{
    timer->stop();

    if(NULL != p_tcpClient)
    {
        qDebug() << "disconnectSocket";
        p_tcpClient->abort();
        p_tcpClient->waitForDisconnected();
        p_tcpClient->disconnectFromHost();
        p_tcpClient->close();
        p_tcpClient->deleteLater();
        p_tcpClient = NULL;
    }

    deleteImgs();
    namelst.clear();
    ui->clientStatusLabel->setText(str_china("客户端就绪"));

    picNametime = 1;
    TotalBytes = 0;
    byteWritten = 0;

    jpgnameNo = 1; //图片名称计数
    time_total = 0.0;//耗时时间
    namelst.clear();//保存的文件名列表
    sizelst.clear();//保存的文件大小列表
    sendDoneFlag = SEND_DONE;//发送结束标志
    emitSigNums = 0;//发送信号的次数
    curstate = STATE_PAUSE;
    //    ui->openButton->setEnabled(true);
    ui->startButton->setEnabled(true);
    //    ui->pauseButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);
}


QString tcpClientFileSend::ReadIpAddr()
{
    QFile file("./serverip.conf");

    if(file.exists())
    {
        QByteArray dataFromFile;
        QString ipaddr;
        file.open(QIODevice::ReadOnly);
        dataFromFile=file.readAll();
        file.close();

#ifdef DEBUG
        qDebug() << "read ip:" << dataFromFile;
#endif
        ipaddr = QString(dataFromFile);
        if(!ipaddr.contains("192.168.1"))
        {
            return QString(DEFAULT_HOSTADDR);
        }

#ifdef DEBUG
        qDebug() << "ipaddr :" << ipaddr;
#endif
        return ipaddr;
    }
    else
    {
        file.open(QIODevice::WriteOnly);
        file.write(DEFAULT_HOSTADDR);
        file.close();
#ifdef DEBUG
        qDebug() << "read ip:" << DEFAULT_HOSTADDR;
#endif
        return QString(DEFAULT_HOSTADDR);
    }
}

void tcpClientFileSend::SaveIpAddr(QString ipaddr)
{
    QFile file("./serverip.conf");
    static uint8_t saveflag = 0;

    if(file.exists() && 0==saveflag)
    {
        file.open(QIODevice::WriteOnly);
        file.write(ipaddr.toLocal8Bit());
        file.close();
        saveflag = 1;
    }
}

void tcpClientFileSend::PrintInfoToFile(QString str)
{
    QFile file("./dbginfo.txt");

    if(file.exists())
    {
        file.open(QIODevice::Append);
        file.write(str.toLocal8Bit());
        file.close();
    }
    else
    {
        file.open(QIODevice::WriteOnly);
        file.write(str.toLocal8Bit());
        file.close();
    }
}

QString tcpClientFileSend::GetVersion(void)
{
    return str_china("桌面传输系统")
            +"\n"
            +str_china("by小魏莱")
            +"\n"
            +version_filetransClient;
}

void tcpClientFileSend::aboutVer()
{
    QMessageBox::information(NULL, str_china("版本"), GetVersion(),NULL,NULL);
    return;
}

void tcpClientFileSend::showVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("录屏传输 ") + QString::fromLocal8Bit(version_filetransClient);
    ui->clientStatusLabel->setText(verinfo);
    //    ui->statusBar->showMessage(verinfo);
}

//检测文本变化
void tcpClientFileSend::textCheck(QString str)
{
    str = str.replace(" ","");
    if(str.isEmpty())
    {
        ui->label->setText(QString::fromLocal8Bit(" 请输入IP地址,并点击开始按钮进行传输"));
        ui->startButton->setEnabled(false);
    }else{
        ui->label->setText(QString::fromLocal8Bit(""));
        ui->startButton->setEnabled(true);
    }

}

void tcpClientFileSend::showTextTransfering()
{
    static quint64 loop = 0;
    switch ((loop++)%7) {
    case 0:
        ui->clientStatusLabel->setText(str_china("传输中"));
        break;
    case 1:
        ui->clientStatusLabel->setText(str_china("传输中."));
        break;
    case 2:
        ui->clientStatusLabel->setText(str_china("传输中.."));
        break;
    case 3:
        ui->clientStatusLabel->setText(str_china("传输中..."));
        break;
    case 4:
        ui->clientStatusLabel->setText(str_china("传输中...."));
        break;
    case 5:
        ui->clientStatusLabel->setText(str_china("传输中....."));
        break;
    case 6:
        ui->clientStatusLabel->setText(str_china("传输中......"));
        break;
    default:
        break;
    }
}

///************************************************/
///*函 数:BtnStartPix                              */
///*入 参:无                                        */
///*出 参:无                                        */
///*返 回:无                                        */
///*功 能:开始图片显示及样式                           */
///*author :wxj                                    */
///*version:1.0                                    */
///*时 间:2015.4.25                                 */
///*************************************************/
//void tcpClientFileSend::BtnStartPix(void)
//{
//    QString str = ":images/start.png";
//    BtnSetPix(str);
//}

///************************************************/
///*函 数:BtnStopPix                               */
///*入 参:无                                        */
///*出 参:无                                        */
///*返 回:无                                        */
///*功 能:停止图片显示及样式                           */
///*author :wxj                                    */
///*version:1.0                                    */
///*时 间:2015.4.25                                 */
///*************************************************/
//void tcpClientFileSend::BtnStopPix(void)
//{
//    QString str = ":images/stop.png";
//    BtnSetPix(str);
//}
