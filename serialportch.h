#ifndef SERIALPORTCH_H
#define SERIALPORTCH_H

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

class serialportch : public QObject
{
    Q_OBJECT
public:
    explicit serialportch(QObject *parent = 0);

private:
    int addr;
    int port;
    int baudRate;
    int stop;
    int step;
    int status,subStatus;
    int error;

    char Binary[128];

    QTimer *timer;
    QSerialPort *com;
    char getChecksum(char *info,int start,int bytes);
    void delay(int d);
    int readTestData(int index, int index1, testDataStr *tds);
    int startTest(int index);
    int stopTest();
    void printBinary(int n);
    int chooseGroup(int index);
    int fillCommon(groupConfig group);
    int fillCommonCH(groupConfig group);
    void sendSysConfig(QByteArray msg);
    void waitStateFun();
    int readMachineStatus();
    void readGroupSetData(int g,int s);

signals:
    void sendCommand(int index,int cmd,testDataStr tds);

private slots:
    void readStatus(void);      //读取下位机状态
    void readGroupData(groupConfig group);
    void serialReadMessage(quint16 addr,quint16 cmd,QByteArray msg);
public slots:
    void openSerial();
    void closeSerial(void);
};

#endif // SERIALPORTCH_H
