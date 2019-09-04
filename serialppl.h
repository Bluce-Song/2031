#ifndef SERIALPPL_H
#define SERIALPPL_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QDebug>
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

class serialPPL : public QObject
{
    Q_OBJECT
public:
    explicit serialPPL(QObject *parent = 0);

private:
    union
    {
        unsigned char uc[4];
        float fda;
    }un_4b;

    int addr;
    int port;
    int baudRate;
    int portPPL;
    int baudRatePPL;
    int portContains;
    int baudRateContains;
    int stop;
    int step;
    int status,subStatus;
    int error;
    int acdc;

    char Binary[128];
    int powerStop;
    int powerOnly;
    int powerDo;
    int powerRun;
    struct timeval pplStart,pplEnd;
    float powerTime;
    QList<int> itemId;
    QList<float> itemTime;
    QList<float> itemMin;
    QList<float> itemMax;
    QTimer *timer;
    QTimer *timerPPL;
    QSerialPort *com;
    QSerialPort *comPPL;
    QSerialPort *comContains;
    char getChecksum(char *info,int start,int bytes);
    void delay(int d);
    int readTestData(int index, int index1, testDataStr *tds);
    int startTest(int index);
    int stopTest();
    void printBinary(int n);
    int chooseGroup(int index);
    int fillCommon(groupConfig group);
    int fillCommonPhase(groupConfig group);
    void sendSysConfig(QByteArray msg);
    void waitStateFun();
    int readMachineStatus();
    void readGroupSetData(int g,int s);    
    void openSerialPPL(void);
    void closeSerialPPL(void);
    void containsAC(int index);
    void containsDC(int index);
    void containsIO(int index);
    void powerItemsTest();
    void alarmEnabled(int index);

signals:
    void sendCommand(int index,int cmd,testDataStr tds);

private slots:
    void readStatus(void);
    int readPowerData(testDataStr *tds);
    void readGroupData(groupConfig group);
    void serialReadMessage(quint16 addr,quint16 cmd,QByteArray msg);
    void powerTimeRun();
public slots:
    void openSerial(void);
    void closeSerial(void);
    void alarmSlots(int index);
};

#endif // SERIALPPL_H
