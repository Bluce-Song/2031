#include "SerialWeiHuangE32.h"

SerialWeiHuangE32::SerialWeiHuangE32(QObject *parent) : QObject(parent)
{
    baudRate = tc.PS.PrintBaudRate;
    port = tc.PS.PrintPort;
    com = NULL;
    stop = 0;
    step = 0;
    status = -1;
    subStatus = -1;
    error = 0;
}

void SerialWeiHuangE32::openSerial()
{
    if(com  ==  NULL)
        com = new QSerialPort(this);
    if(com->isOpen())
        com->close();
    QString portname;
    if(port >= 10)
    {
        portname.append("\\\\.\\").append("com"+QString::number(port));
    }else{
        portname.append("com"+QString::number(port));
    }

    com->setPortName(portname);

//    com->setPortName("com"+QString::number(port));
    com->setBaudRate(baudRate);     //波特率
    com->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    com->setDataBits(QSerialPort::Data8);        //数据位设置，设置为8位数据位
    com->setStopBits(QSerialPort::OneStop);      //停止位设置，设置为1位停止位
    com->setFlowControl(QSerialPort::NoFlowControl); //控制流
    bool ok = com->open(QSerialPort::ReadWrite);

    if(ok)
    {
        com->setDataTerminalReady(true);
        com->setRequestToSend(false);
        tc.portOpen = 1;
    }else{
        tc.portOpen = 0;
    }
}

void SerialWeiHuangE32::closeSerial()
{
    if(com != NULL && com->isOpen())
        com->close();
    tc.portOpen = 0;
}

void SerialWeiHuangE32::SerialPrintout(QStringList list, QList<testDataStr> datalist)
{
    if(list.isEmpty() || datalist.isEmpty())
        return;
    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QStringList PrintDataList;//定义炜煌E32打印字符串列表
    PrintDataList<<list.at(0)<<saveTime;

    for(int i=0;i<datalist.size();i++)
    {
        QString name = "";
        int result = 0;
        QStringList data;
        switch(datalist.at(i).model)
        {
            case 0:
                name = tr("无效");
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                break;
            case 1:
                name = tr("交耐");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[0].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[0].prec[1]);
                data<<"";
                data<<rangeSets[0].unit[0];
                data<<rangeSets[0].unit[1];
                data<<"";
                break;
            case 2:
                name = tr("直耐");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[1].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[1].prec[1]);
                data<<"";
                data<<rangeSets[1].unit[0];
                data<<rangeSets[1].unit[1];
                data<<"";
                break;
            case 3:
                name = tr("绝缘");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[2].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[2].prec[1]);
                data<<"";
                data<<rangeSets[2].unit[0];
                data<<rangeSets[2].unit[1];
                data<<"";
                break;
            case 4:
                name = tr("接地");
                data<<QString::number(datalist.at(i).current,10,rangeSets[3].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[3].prec[1]);
                data<<"";
                data<<rangeSets[3].unit[3];
                data<<rangeSets[3].unit[1];
                data<<"";
                break;
            case 5:
                name = tr("泄漏");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[4].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[4].prec[1]);
                if(tc.phase == 1)
                    data<<QString::number(datalist.at(i).pCurrent,10,0);
                else
                    data<<"";
                data<<rangeSets[4].unit[0];
                data<<rangeSets[4].unit[1];
                if(tc.phase == 1)
                    data<<"W";
                else
                    data<<"";
                break;
            case 6:
                name = tr("功率");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[5].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[5].prec[3]);
                data<<QString::number(datalist.at(i).power,10,rangeSets[5].prec[1]);
                data<<rangeSets[5].unit[0];
                data<<rangeSets[5].unit[3];
                data<<rangeSets[5].unit[1];
                break;
            case 7:
                name = tr("低启");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[6].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[6].prec[1]);
                data<<QString::number(datalist.at(i).power,10,rangeSets[6].prec[3]);
                data<<rangeSets[6].unit[0];
                data<<rangeSets[6].unit[1];
                data<<rangeSets[6].unit[3];
                break;
            case 8:
                name = tr("开短");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[7].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[7].prec[1]);
                data<<"";
                data<<rangeSets[7].unit[0];
                data<<rangeSets[7].unit[1];
                data<<"";
                break;
        }

        if(datalist.at(i).flag1 == 1)
        {
            result = 1;
        }else{
            result = 0;
        }

        QString cd = list.at(4+datalist.at(i).step);
        QStringList cdList = cd.split(",");

        PrintDataList<<name+ " " +cdList.at(0)+cdList.at(1)+" "+cdList.at(2)+"~"+cdList.at(4)
               +cdList.at(5);
        if (result == 1)
            PrintDataList<< "OK   "+data.at(0)+data.at(3)+" "+data.at(1)+data.at(4)+" "+data.at(2)+data.at(5);
        else
            PrintDataList<< "NG   "+data.at(0)+data.at(3)+" "+data.at(1)+data.at(4)+" "+data.at(2)+data.at(5);
    }


    QByteArray sendData;
    QString dataStr;
    int num = PrintDataList.count();
    for(int i=num;i>0;i--)
    {
        sendData.clear();
        dataStr.clear();
        dataStr = PrintDataList.at(i-1);
        sendData = ToGbk(dataStr);
        sendData.append(0x0a);
        qDebug()<<sendData;
        com->write(sendData);
//        delay(50);
    }
    sendData.clear();
    sendData.append(0x1b);
    sendData.append(0x4a);
    sendData.append(0x40);
    com->write(sendData);

}

void SerialWeiHuangE32::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents();
    }
}
QByteArray SerialWeiHuangE32::ToGbk(const QString &inStr)
{
    QTextCodec *gbk = QTextCodec::codecForName("GB18030");
    return gbk->fromUnicode(inStr);
}
