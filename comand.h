#ifndef COMAND_H
#define COMAND_H

#include "loginsetinfo.h"
#include <QObject>
#include <QString>
#include <QChar>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class comand
{
public:
    comand();
    int machineState();
    int startTest();
    int touch(int port);
private:
    QSerialPort *w_port;    //通信串口
    QString binToHex(const QString &strBin, bool bIsUpper = false);
    QString hexToBin(const QString &strHex);
    unsigned char getChecksum(unsigned char *info, int start, int bytes);
};

#endif // COMAND_H
