#ifndef SCANNER_H
#define SCANNER_H

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

class scanner : public QObject
{
    Q_OBJECT
public:
    explicit scanner(QObject *parent = 0);
    int stop;

private:
    int addr;
    int port;
    int baudRate;
    int portOpen;
    int total;

    char barCodeRead[4096];

    QTimer *timer;
    QSerialPort *com;
    void delay(int d);
    void readBarCode();    

signals:
    void sendBarCode(QString barCode);

public slots:
    void openScanner(void);
    void closeScanner(void);
    void readStop();
    void readScanner(int cmd);
private slots:
    void readScannerData();
};

#endif // SCANNER_H
