#ifndef SERIALWEIHUANGE32_H
#define SERIALWEIHUANGE32_H

#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QTextCodec>
#include <QStringList>
#include <QApplication>
#include <QEventLoop>
#include <QCoreApplication>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <sys/time.h>
#include "loginsetinfo.h"
#include <QFile>
#include <QDate>
#include <QDir>
#include <QUrl>
#include <qtsoap.h>
#include <QAxObject>
#include <QObject>
#include <QVariant>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QCoreApplication>

extern testConfig tc;
extern range_Item rangeSets[8];

class SerialWeiHuangE32 : public QObject
{
    Q_OBJECT
public:
    explicit SerialWeiHuangE32(QObject *parent = 0);
private:
    int port;
    int baudRate;
    int stop;
    int step;
    int status,subStatus;
    int error;

    char Binary[128];

    QTimer *timer;
    QSerialPort *com;
    void delay(int d);
    QByteArray ToGbk(const QString &inStr);

signals:

public slots:
    void openSerial();
    void closeSerial(void);
    void SerialPrintout(QStringList list, QList<testDataStr> datalist);
};

#endif // SERIALWEIHUANGE32_H
