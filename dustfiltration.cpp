#include "dustfiltration.h"

dustFiltration::dustFiltration(QObject *parent) : QObject(parent)
{
    port1 = tc.SPS.psPort1;
    port2 = tc.SPS.psPort2;
    baudRate1 = tc.SPS.psBaudRate1;
    baudRate2 = tc.SPS.psBaudRate2;
//    qDebug()<<port1<<port2<<baudRate1<<baudRate2;
    com1 = NULL;
    com2 = NULL;
    sum = 0;
    psData1 = 0.0;
    psData2 = 0.0;
}

void dustFiltration::openSerial()
{
    if(com1 == NULL)
        com1 = new QSerialPort(this);

    if(com1->isOpen())
        com1->close();

    QString portname;
    if(port1 >= 10)
    {
        portname.append("\\\\.\\").append("com"+QString::number(port1));
    }else{
        portname.append("com"+QString::number(port1));
    }

    com1->setPortName(portname);
    com1->setBaudRate(baudRate1);     //波特率
    com1->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    com1->setDataBits(QSerialPort::Data8);        //数据位设置，设置为8位数据位
    com1->setStopBits(QSerialPort::OneStop);      //停止位设置，设置为1位停止位
    com1->setFlowControl(QSerialPort::NoFlowControl); //控制流
    bool ok = com1->open(QSerialPort::ReadWrite);

    if(ok)
    {
        tc.psPort1Open = 1;
        com1->setDataTerminalReady(true);
        com1->setRequestToSend(false);
        connect(com1, SIGNAL(readyRead()), this, SLOT(readSensor1Data()));
    }else{
        tc.psPort1Open = 0;
    }

    if(com2 == NULL)
        com2 = new QSerialPort(this);

    if(com2->isOpen())
        com2->close();

    portname.clear();
    if(port2 >= 10)
    {
        portname.append("\\\\.\\").append("com"+QString::number(port2));
    }else{
        portname.append("com"+QString::number(port2));
    }

    com2->setPortName(portname);
    com2->setBaudRate(baudRate2);     //波特率
    com2->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    com2->setDataBits(QSerialPort::Data8);        //数据位设置，设置为8位数据位
    com2->setStopBits(QSerialPort::OneStop);      //停止位设置，设置为1位停止位
    com2->setFlowControl(QSerialPort::NoFlowControl); //控制流
    ok = com2->open(QSerialPort::ReadWrite);

    if(ok)
    {
        tc.psPort2Open = 1;
        com2->setDataTerminalReady(true);
        com2->setRequestToSend(false);
        connect(com2, SIGNAL(readyRead()), this, SLOT(readSensor2Data()));
    }else{
        tc.psPort2Open = 0;
    }
}

void dustFiltration::closeSerial()
{
//    timer->stop();
    if(com1 != NULL && com1->isOpen())
        com1->close();

    if(com2 != NULL && com2->isOpen())
        com2->close();
}

void dustFiltration::readSensor1Data()
{
//    QString sn = buffer;
//    emit sendSensor1Data(sn);
    QByteArray buffer;
    if(com1->bytesAvailable() >= 32)
    {
        buffer = com1->readAll();
        float a = (quint8(buffer.at(6))<<8|quint8(buffer.at(7)))/1.0;

        if(a >= 0.0)
            psData1 = a;
//        QString s = QString::number(a,10,1);
        sendSensor1Data(psData1);
    }
}

void dustFiltration::readSensor2Data()
{
    QByteArray buffer;
    if(com2->bytesAvailable() >= 32)
    {
        buffer = com2->readAll();
        float a = (quint8(buffer.at(6))<<8|quint8(buffer.at(7)))/1.0;

        if(a >= 0.0)
            psData2 = a;
//        QString s = QString::number(a,10,1);
        sendSensor2Data(psData2);
//        float r = 0.0;
//        if(psData1 == 0.0)
//        {
//            r = 0.0;
//        }else{
////            r = QString::number(((psData1 - psData2)/psData1)*100.0,10,1)+" %";
//            r = ((psData1 - psData2)/psData1)*100.0;
//        }
//        sendSensor3Data(r);
    }
}
