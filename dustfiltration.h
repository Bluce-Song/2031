#ifndef DUSTFILTRATION_H
#define DUSTFILTRATION_H

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

extern testConfig tc;
class dustFiltration : public QObject
{
    Q_OBJECT
public:
    explicit dustFiltration(QObject *parent = 0);

private:
    int port1;
    int baudRate1;
    int port2;
    int baudRate2;

    int sum;
    float psData1;
    float psData2;

//    QTimer *timer;
    QSerialPort *com1;
    QSerialPort *com2;
    QString barCodeScan;
//    char getChecksum(char *info,int start,int bytes);
//    void delay(int d);

signals:
    void sendSensor1Data(float barCode);
    void sendSensor2Data(float barCode);
    void sendSensor3Data(float barCode);

public slots:
    void openSerial();
    void closeSerial(void);
private slots:
    void readSensor1Data();
    void readSensor2Data();
};

#endif // DUSTFILTRATION_H
