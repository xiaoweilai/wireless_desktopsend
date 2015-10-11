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
    //����ı��仯
    void textCheck(QString str);
    void aboutVer();
    QString GetVersion(void);
    void PrintInfoToFile(QString str);
    void SaveIpAddr(QString ipaddr);
    QString ReadIpAddr();
    //�Ͽ�����
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

    /*************[���紫��]**********************/
    qint64 writeNetData(const QByteArray &iData);
private:
    Ui::ScreenCap *ui;
    static int isStarted;
    static unsigned int mNo;

    /*************[���紫��]**********************/
    QTcpSocket *p_tcpClient;
    QByteArray outBlock;       //����һ�η��͵�����
    QByteArray outBlkData;//����һ�η��͵�����
    qint64 TotalBytes;
    qint64 byteWritten;
    qint64 bytesToWrite;
    quint64 picNametime;
    QTimer *pNetSendTimer;
    qint64  loadSize;          //����ʼ��Ϊһ��4Kb�ĳ���
    QBuffer buffer;//�����������ݵ�һ������
    QByteArray tmpbyte;//������������n��������
    quint8 sendDoneFlag;//�����Ƿ������


    /*************[����log]**********************/
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
    void pause();//��ͣ
    void startTransfer();
    void parseImage();
    void displayErr(QAbstractSocket::SocketError socketError);
    void openFile();
    QImage grabframeGeometry();
    QImage grabDeskScreen();
    void ShutDownAll();
private:
    void deleteImgs(); //�رպ�ɾ���ļ���


private:
    QString fileName;
    QFile *localFile;

    QTimer *timer;//���Ͷ�ʱ��
    QVector<QImage> imageVec;
    QImage fileImage;

    QString dirname;//�ļ�������
    quint64 jpgnameNo;
    double time_total;//��ʱʱ��
    QList<QString> namelst; //�����ļ����б�
    QList<quint32> sizelst; //�����ļ���С�б�
    QByteArray outBlockFile;//�ļ��ֽ�����

    enum{
        SEND_DONE = 0,
        SEND_ING,
        SEND_UNKOWN
    };

    quint8 emitSigNums;//�����źŵĴ���
    enum{
        STATE_START = 0,
        STATE_PAUSE,
        STATE_UNKOWN
    };
    quint8 curstate;//Ŀǰ��ʱ��״̬
    QVector<QByteArray> imgVecArray;//�洢ͼƬ�Ļ��棬vector����
    QList<QByteArray>   imgLstArray;//�洢ͼƬ�Ļ��棬lst����

    QScreen *pscreen;
signals:
    void emitImgZeroSignal();//��namelstΪ��ʱ�������źŵ���parseImage();


};

#endif // MAINWINDOW_H
