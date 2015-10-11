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

//#define DEBUG    /* ������Ϣ */
//#define TIMETEST /* ��ʱ���� */

const char version_filetransClient[]="v2.0";


#if 1
//���뺺��
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

#define IMGPOOLSIZE 3 /* ͼƬ������С */

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

    jpgnameNo = 1; //ͼƬ���Ƽ���
    time_total = 0.0;//��ʱʱ��
    namelst.clear();//������ļ����б�
    sizelst.clear();//������ļ���С�б�
    sendDoneFlag = SEND_DONE;//���ͽ�����־
    emitSigNums = 0;//�����źŵĴ���
    curstate = STATE_PAUSE;//��ǰ״̬����������ͣ
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
//����
void tcpClientFileSend::openFile()
{
    ui->startButton->setEnabled(true);
    ui->clientStatusLabel->setText(str_china("������ʼԶ�̴���"));
    //����tcpsocket
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


    ui->clientStatusLabel->setText(str_china("������..."));
    p_tcpClient->connectToHost(ui->lineEditHost->text(),
                               QString(DEFAULT_PORT).toInt());
    p_tcpClient->setSocketOption(QAbstractSocket::LowDelayOption, 1);//�Ż�Ϊ����ӳ٣������1�������ø��Ż���


    //waitForConnected()�ȴ�����֪���������ȴ�ʱ�䡣������ӽ�����������true�����򷵻�false��
    //������falseʱ���Ե���error��ȷ���޷����ӵ�ԭ��
    if(!p_tcpClient->waitForConnected(3000))
    {
        qDebug() <<"here:" << p_tcpClient;
        if(NULL != p_tcpClient)
        {
            qDebug() <<"Error: "<<p_tcpClient->errorString();
            p_tcpClient->deleteLater();
            p_tcpClient = NULL;

            QMessageBox::information(this,str_china("����"),
                                     str_china("�������´�������ʧ��"));
        }
        curstate = STATE_PAUSE;
        timer->stop();

        //        ui->openButton->setEnabled(true);
        ui->startButton->setEnabled(true);
        //        ui->pauseButton->setEnabled(false);
        ui->disconnectButton->setEnabled(false);
        ui->clientStatusLabel->setText(str_china("����ʧ�ܣ���ȷ������IP����"));
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
        ui->clientStatusLabel->setText(str_china("���ӳɹ�"));
    }

}
//��ͣ
void tcpClientFileSend::pause()
{
    curstate = STATE_PAUSE;
    ui->startButton->setEnabled(true);
    //    ui->pauseButton->setEnabled(false);
    ui->disconnectButton->setEnabled(true);
    ui->clientStatusLabel->setText(str_china("��ͣ��..."));
}

/* һ�����ӽ����ɹ���QTcpSocket�ཫ����connected��Ϣ���̶�����
startTransfer()�ۺ�����
 */
void tcpClientFileSend::startTransfer()
{
    if(STATE_PAUSE == curstate)
    {
        timer->stop();
        return;
    }

#ifdef TIMETEST /* ��ʱ���� */
    QTime time;
    time.start(); //��ʼ��ʱ����msΪ��λ

#endif /* ��ʱ���� */

    //    ui->clientStatusLabel->setText(str_china("���ӳɹ�"));

    if(namelst.count() > IMGPOOLSIZE)
    {
        //        if(socketError == QTcpSocket::RemoteHostClosedError)
        //        {

        //        }
        return;
    }
    emitSigNums = 0;//�����źŹ���

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

#ifdef TIMETEST /* ��ʱ���� */

    int time_Diff = time.elapsed(); //���ش��ϴ�start()��restart()��ʼ������ʱ����λms
    //���·����ǽ�msתΪs
    float f = time_Diff / 1000.0;
    time_total += f;

    qDebug() << "save  elaspe:" <<time_Diff <<"ms";
    qDebug() << "current time:" <<time_total <<"s";

#endif /* ��ʱ���� */


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

    sendDoneFlag = SEND_ING;//������

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
�����ļ����ݸ�ʽ ��

1.�ܳ��� (8bytes) -- ���ֽ�(�ļ���С + 8�ֽ� + �ļ���)
2.�ļ�������(qint64() 8Bytes)
3.�ļ���
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

    //�����ļ�����
    sendOut <<qint64(0) <<qint64(0) <<readFname;
    //TotalBytesΪ�����ݳ��ȣ���������������+�ļ�������+�ļ�����
    TotalBytes += outBlock.size(); //����ͼƬ���Ƴ���
    sendOut.device()->seek(0);

    //���ֽ�(�ļ���С + 8�ֽ� + �ļ���) ��
    sendOut << TotalBytes << qint64((outBlock.size() - sizeof(qint64)*2));

    bytesToWrite = TotalBytes - p_tcpClient->write(outBlock);//�����Ʒ�����ʣ��ͼƬ��С
    //    ui->clientStatusLabel->setText(str_china("������..."));
    showTextTransfering();
#ifdef DEBUG
    qDebug() << "TotalBytes:" << TotalBytes;
#endif

    //    file.close();
    //    dir.remove(readFname);//ɾ���ļ�


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
            ui->clientStatusLabel->setText(str_china("��ͣ��..."));
            return;
        }else{
            ui->clientStatusLabel->setText(str_china("������..."));
        }
        parseImage();
    }



}

void tcpClientFileSend::displayErr(QAbstractSocket::SocketError socketError)
{
    qDebug() << "display err";
    if(NULL == p_tcpClient)
    {
        QMessageBox::information(this,str_china("����"),
                                 str_china("�������´�������ʧ��"));
    }else{
        QMessageBox::information(this,str_china("����"),
                                 str_china("�������´��� %1")
                                 .arg(p_tcpClient->errorString()));
    }

    disconnectSocket();


#ifdef SHOWCURSOR
    QApplication::restoreOverrideCursor();
#endif
}

//����ߴ�
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

//����ߴ�
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

//�Ͽ�����
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
    ui->clientStatusLabel->setText(str_china("�ͻ��˾���"));

    picNametime = 1;
    TotalBytes = 0;
    byteWritten = 0;

    jpgnameNo = 1; //ͼƬ���Ƽ���
    time_total = 0.0;//��ʱʱ��
    namelst.clear();//������ļ����б�
    sizelst.clear();//������ļ���С�б�
    sendDoneFlag = SEND_DONE;//���ͽ�����־
    emitSigNums = 0;//�����źŵĴ���
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
    return str_china("���洫��ϵͳ")
            +"\n"
            +str_china("byСκ��")
            +"\n"
            +version_filetransClient;
}

void tcpClientFileSend::aboutVer()
{
    QMessageBox::information(NULL, str_china("�汾"), GetVersion(),NULL,NULL);
    return;
}

void tcpClientFileSend::showVerion(void)
{
    QString verinfo = QString::fromLocal8Bit("¼������ ") + QString::fromLocal8Bit(version_filetransClient);
    ui->clientStatusLabel->setText(verinfo);
    //    ui->statusBar->showMessage(verinfo);
}

//����ı��仯
void tcpClientFileSend::textCheck(QString str)
{
    str = str.replace(" ","");
    if(str.isEmpty())
    {
        ui->label->setText(QString::fromLocal8Bit(" ������IP��ַ,�������ʼ��ť���д���"));
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
        ui->clientStatusLabel->setText(str_china("������"));
        break;
    case 1:
        ui->clientStatusLabel->setText(str_china("������."));
        break;
    case 2:
        ui->clientStatusLabel->setText(str_china("������.."));
        break;
    case 3:
        ui->clientStatusLabel->setText(str_china("������..."));
        break;
    case 4:
        ui->clientStatusLabel->setText(str_china("������...."));
        break;
    case 5:
        ui->clientStatusLabel->setText(str_china("������....."));
        break;
    case 6:
        ui->clientStatusLabel->setText(str_china("������......"));
        break;
    default:
        break;
    }
}

///************************************************/
///*�� ��:BtnStartPix                              */
///*�� ��:��                                        */
///*�� ��:��                                        */
///*�� ��:��                                        */
///*�� ��:��ʼͼƬ��ʾ����ʽ                           */
///*author :wxj                                    */
///*version:1.0                                    */
///*ʱ ��:2015.4.25                                 */
///*************************************************/
//void tcpClientFileSend::BtnStartPix(void)
//{
//    QString str = ":images/start.png";
//    BtnSetPix(str);
//}

///************************************************/
///*�� ��:BtnStopPix                               */
///*�� ��:��                                        */
///*�� ��:��                                        */
///*�� ��:��                                        */
///*�� ��:ֹͣͼƬ��ʾ����ʽ                           */
///*author :wxj                                    */
///*version:1.0                                    */
///*ʱ ��:2015.4.25                                 */
///*************************************************/
//void tcpClientFileSend::BtnStopPix(void)
//{
//    QString str = ":images/stop.png";
//    BtnSetPix(str);
//}
