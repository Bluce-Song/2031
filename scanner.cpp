#include "scanner.h"

scanner::scanner(QObject *parent) : QObject(parent)
{
    addr = tc.SPS.machineAddr;
    baudRate = tc.SPS.sBaudRate;
    port = tc.SPS.sPort;
    total = 0;
    com = NULL;
}

void scanner::openScanner()
{
    if (com  ==  NULL)
        com = new QSerialPort(this);

    if (com->isOpen())
        com->close();

    com->setPortName("com"+QString::number(port));
    com->setBaudRate(baudRate);     //波特率
    com->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    com->setDataBits(QSerialPort::Data8);        //数据位设置，设置为8位数据位
    com->setStopBits(QSerialPort::OneStop);      //停止位设置，设置为1位停止位
    com->setFlowControl(QSerialPort::NoFlowControl); //控制流
    com->setReadBufferSize(1024);                //缓冲区大小
    bool ok = com->open(QSerialPort::ReadWrite);

    if(ok)
    {
        portOpen = 1;
        tc.sPortOpen = 1;
        connect(com, SIGNAL(readyRead()), this, SLOT(readScannerData()));
    }else{
        portOpen = 0;
        tc.sPortOpen = 0;
    }
}

void scanner::readScannerData()
{
    QByteArray buffer;

    buffer = com->readAll();
    QString sn = buffer;
    emit sendBarCode(sn);
}

void scanner::readStop()
{
    stop = 1;
}

void scanner::readScanner(int cmd)
{
    if(cmd == 0)
        closeScanner();
    else
        openScanner();
}

void scanner::closeScanner()
{
    stop = 1;
    if(com != NULL && com->isOpen())
        com->close();
    portOpen = 0;
    tc.sPortOpen = 0;
}

void scanner::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents();
    }
}

void scanner::readBarCode()
{
    int i,j,sum,len;
    char rb[4096];

    sum = 0;
    stop = 0;
    for(;;)
    {
        if(stop == 1)
            break;

        QCoreApplication::processEvents();
        j = com->bytesAvailable();

        if(j > 4095 || j <= 0)
            continue;

        if(stop == 1)
            break;
        len = com->read(rb,j);

        for(i=0;i<j;i++)
        {
            barCodeRead[total+i] = rb[i];
            QCoreApplication::processEvents();
            if(rb[i] == 13)
            {
                barCodeRead[total+i] = 0;
                j = 0;
                total = 0;
                QString barCode(barCodeRead);
                emit sendBarCode(barCode);
                strcpy(barCodeRead,"");
                break;
            }
        }
        QCoreApplication::processEvents();
        total += j;
    }
}
