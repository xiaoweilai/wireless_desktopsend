#ifndef TCPCLIENTFILESEND_H
#define TCPCLIENTFILESEND_H

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
class tcpClientFileSend;
}

class tcpClientFileSend : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit tcpClientFileSend(QWidget *parent = 0);
    ~tcpClientFileSend();
    

public slots:
    void start();
    void pause();//��ͣ
    void startTransfer();
    void parseImage();
    void updateClientProgress(qint64 numBytes);
    void displayErr(QAbstractSocket::SocketError socketError);
    void openFile();
    void aboutVer();//����汾��Ϣ
    QString GetVersion(void);
    QImage grabframeGeometry();
    QImage grabDeskScreen();
    void ShutDownAll();
    void disconnectSocket();//�Ͽ�����
    QString ReadIpAddr();
    void SaveIpAddr(QString ipaddr);
    void PrintInfoToFile(QString str);

    void textCheck(QString str);
    void showTextTransfering();
private:
    void deleteImgs(); //�رպ�ɾ���ļ���
    void showVerion();


private:
    Ui::tcpClientFileSend *ui;

    QTcpSocket *p_tcpClient;

    qint64 TotalBytes;
    qint64 byteWritten;
    qint64 bytesToWrite;
    QString fileName;
    QFile *localFile;
    QByteArray outBlock;

    QTimer *timer;//���Ͷ�ʱ��
    QVector<QImage> imageVec;
    QImage fileImage;
    quint64 picNametime;
    QBuffer buffer;

    QString dirname;//�ļ�������
    quint64 jpgnameNo;
    double time_total;//��ʱʱ��
    QList<QString> namelst; //�����ļ����б�
    QList<quint32> sizelst; //�����ļ���С�б�
    QByteArray outBlockFile;//�ļ��ֽ�����
    quint8 sendDoneFlag;//���ͽ�����־
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

#endif // TCPCLIENTFILESEND_H
