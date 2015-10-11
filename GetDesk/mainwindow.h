#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QBuffer>
#include <QFile>
#include <QMainWindow>
#include <QDialog>
#include <QtGui>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QHostAddress>
#include <QScreen>

#define STREAM_PIC_FORT_PNG "PNG"

#if 0 //ok 2pic/s
#define STREAM_PIC_FORT "PNG"
#define SUFIXNAME       "png"
#elif 0 //err
#define STREAM_PIC_FORT "BMP"
#define SUFIXNAME       "bmp"
#elif 0  //ok, 6pic/s
#define STREAM_PIC_FORT "JPG"
#define SUFIXNAME       "jpg"
#elif 1 //ok, 6pic/s
#define STREAM_PIC_FORT "JPEG"
#define SUFIXNAME       "jpeg"
#elif 0 //err,size too little
#define STREAM_PIC_FORT "GIF"
#define SUFIXNAME       "gif"
#elif 0 //err,too big
#define STREAM_PIC_FORT "TIFF"
#define SUFIXNAME       "tiff"
#elif 1 //not show
#define STREAM_PIC_FORT "PPM"
#define SUFIXNAME       "ppm"
#endif



namespace Ui {
class ScreenCap;
}

enum
{
    RET_SUCESS,
    RET_FAIL,
    RET_UNKNOWN
};

enum{
    SEND_UNDO,
    SEND_DONE,
    SEND_UNKNOWN
};

enum
{
    STAT_STOPPED,
    STAT_RESTOP,
    STAT_RESTART,
    STAT_STARTED,
};
class ScreenCap : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenCap(QWidget *parent = 0);
//    void StartTransferThread(void);
    ~ScreenCap();

    void SetThreadFlag(quint8 flag);
    quint8 GetThreadFlag(void);
//    CapThread *pCapThread;
    int CreateCapturethread();
    int CreateTcpSocket();

private slots:
    void on_pushButtonStart_clicked();
    void LineTextTips(QString str);
    void StopActionSets();
    void showVerion(void);
    void showTextStop();
    void showTextStart();
    void StartCapScreen();
    //检测文本变化
    void textCheck(QString str);
    void aboutVer();
    QString GetVersion(void);
    void PrintInfoToFile(QString str);
    void SaveIpAddr(QString ipaddr);
    QString ReadIpAddr();
    //断开连接
    void disconnectSocket();
private:
    void showTextTransfering();
    void showTextTransferingNo(quint64 num);
    void showTextTransferOver();
    void showStateBarInfo(const char *pstr);
    void showAppVerion(void);
    void showTextClickToStart(void);
    void showTextClickOverToReStart(void);
    void showTextConnecting(void);
    void BtnStartPix(void);
    void BtnStopPix(void);
    void BtnSetPix(QString str);
    void BtnEnable(void);
    void BtnDisable(void);
    int CaptureScreenOn();
    void StopCapScreen();

    /*************[网络传输]**********************/
    qint64 writeNetData(const QByteArray &iData);
private:
    Ui::ScreenCap *ui;
    static int isStarted;
    static unsigned int mNo;

    /*************[网络传输]**********************/
    QTcpSocket *p_tcpClient;
    QByteArray outBlock;       //缓存一次发送的数据
    QByteArray outBlkData;//缓存一次发送的数据
    qint64 TotalBytes;
    qint64 byteWritten;
    qint64 bytesToWrite;
    quint64 picNametime;
    QTimer *pNetSendTimer;
    qint64  loadSize;          //被初始化为一个4Kb的常量
    QBuffer buffer;//传输网络数据的一个过程
    QByteArray tmpbyte;//保存网络数据n个的内容
    quint8 sendDoneFlag;//数据是否发生完毕


    /*************[保存log]**********************/
    QFile *plogFile;
    QString logfilename;
    QFile *pdataFile;
    QString datafilename;

private:
    void LogInitLog();
    void LogWriteFile(QString str);
    void LogWriteDataFile(const QByteArray &data);
    void LogWriteFileComWithName(QString filename,QByteArray data);

signals:
    void emitCtrlPthreadStart();
    void emitCtrlPthreadStop();
    void emitCtrlPthreadQuit();


public slots:
    int  CheckIPAddrValid(QString ipaddr);
    int  WithNetworkInit(QString ipaddr);
    void displayNetErr(QAbstractSocket::SocketError socketError);
    QString getSockState(QAbstractSocket::SocketState state);
    void updateClientProgress(qint64 numBytes);
    void NetSendData();
    void TimerSets();
    void MergeSendMessage();
    void InfoRecvMessage();


public slots:
    void start();
    void pause();//暂停
    void startTransfer();
    void parseImage();
    void displayErr(QAbstractSocket::SocketError socketError);
    void openFile();
    QImage grabframeGeometry();
    QImage grabDeskScreen();
    void ShutDownAll();
private:
    void deleteImgs(); //关闭后删除文件夹


private:
    QString fileName;
    QFile *localFile;

    QTimer *timer;//发送定时器
    QVector<QImage> imageVec;
    QImage fileImage;

    QString dirname;//文件夹名称
    quint64 jpgnameNo;
    double time_total;//耗时时间
    QList<QString> namelst; //保存文件名列表
    QList<quint32> sizelst; //保存文件大小列表
    QByteArray outBlockFile;//文件字节序列

    enum{
        SEND_DONE = 0,
        SEND_ING,
        SEND_UNKOWN
    };

    quint8 emitSigNums;//发送信号的次数
    enum{
        STATE_START = 0,
        STATE_PAUSE,
        STATE_UNKOWN
    };
    quint8 curstate;//目前定时器状态
    QVector<QByteArray> imgVecArray;//存储图片的缓存，vector类型
    QList<QByteArray>   imgLstArray;//存储图片的缓存，lst类型

    QScreen *pscreen;
signals:
    void emitImgZeroSignal();//当namelst为空时，发送信号调用parseImage();


};

#endif // MAINWINDOW_H
