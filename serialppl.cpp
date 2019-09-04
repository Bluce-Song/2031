#include "serialppl.h"

serialPPL::serialPPL(QObject *parent) : QObject(parent)
{
    addr = tc.SPS.machineAddr;
    baudRate = tc.SPS.baudRate;
    port = tc.SPS.port;        
    com = NULL;
    comPPL = NULL;
    comContains = NULL;
    stop = 0;
    step = 0;
    status = -1;
    subStatus = -1;
    error = 0;
    powerDo = 0;
    powerRun = 0;
    powerTime = 0.0;
    powerOnly = 0;
    powerStop = 0;
    acdc = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readStatus()));

    timerPPL = new QTimer(this);
    connect(timerPPL, SIGNAL(timeout()), this, SLOT(powerTimeRun()));
}

void serialPPL::openSerial()
{
    if(com == NULL)
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

    timer->start(100);
    openSerialPPL();
}

void serialPPL::closeSerial()
{
    timer->stop();
    if(com != NULL && com->isOpen())
        com->close();
    tc.portOpen = 0;
    closeSerialPPL();
}

void serialPPL::openSerialPPL()
{
    if(comPPL == NULL)
        comPPL = new QSerialPort(this);

    if(comPPL->isOpen())
        comPPL->close();

    portPPL = tc.SPS.pplPort;
    baudRatePPL = tc.SPS.pplBaudRate;

    QString portname;
    if(portPPL >= 10)
    {
        portname.append("\\\\.\\").append("com"+QString::number(portPPL));
    }else{
        portname.append("com"+QString::number(portPPL));
    }

    comPPL->setPortName(portname);

//    comPPL->setPortName("com"+QString::number(port));
    comPPL->setBaudRate(baudRatePPL);     //波特率
    comPPL->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    comPPL->setDataBits(QSerialPort::Data8);        //数据位设置，设置为8位数据位
    comPPL->setStopBits(QSerialPort::OneStop);      //停止位设置，设置为1位停止位
    comPPL->setFlowControl(QSerialPort::NoFlowControl); //控制流
    bool ok = comPPL->open(QSerialPort::ReadWrite);

    if(ok)
    {
        comPPL->setDataTerminalReady(true);
        comPPL->setRequestToSend(false);
        tc.pPortOpen = 1;
    }else{
        tc.pPortOpen = 0;
    }

    if(comContains == NULL)
        comContains = new QSerialPort(this);

    if(comContains->isOpen())
        comContains->close();

    portContains = tc.SPS.cPort;
    baudRateContains = tc.SPS.cBaudRate;

    portname.clear();
    if(portContains >= 10)
    {
        portname.append("\\\\.\\").append("com"+QString::number(portContains));
    }else{
        portname.append("com"+QString::number(portContains));
    }

    comContains->setPortName(portname);
    comContains->setBaudRate(baudRateContains);     //波特率
    comContains->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    comContains->setDataBits(QSerialPort::Data8);        //数据位设置，设置为8位数据位
    comContains->setStopBits(QSerialPort::OneStop);      //停止位设置，设置为1位停止位
    comContains->setFlowControl(QSerialPort::NoFlowControl); //控制流
    ok = comContains->open(QSerialPort::ReadWrite);

    if(ok)
    {
        comContains->setDataTerminalReady(true);
        comContains->setRequestToSend(false);
        tc.cPortOpen = 1;
    }else{
        tc.cPortOpen = 0;
    }
}

void serialPPL::closeSerialPPL()
{
//    timerPPL->stop();
    if(comPPL != NULL && comPPL->isOpen())
        comPPL->close();
    tc.pPortOpen = 0;
}

char serialPPL::getChecksum(char *info,int start,int bytes)
{
    int i;
    int checksum = 0;

    for(i=start;i<(bytes+start);i++)
    {
        checksum += info[i];
    }
    return checksum&0xff;
}

void serialPPL::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents();
    }
}

void serialPPL::containsAC(int index)
{
    char cmd[15];

    if(index == 1)
    {
        cmd[0] = 0x7B;
        cmd[1] = 0x0D;
        cmd[2] = 0x00;
        cmd[3] = 0x10;
        cmd[4] = 0x03;
        cmd[5] = 0x00;
        cmd[6] = 0x00;
        cmd[7] = 0x00;
        cmd[8] = 0x00;
        cmd[9] = 0x00;
        cmd[10] = 0x03;
        cmd[11] = 0x23;
        cmd[12] = 0x7D;
    }else{
        cmd[0] = 0x7B;
        cmd[1] = 0x0D;
        cmd[2] = 0x00;
        cmd[3] = 0x10;
        cmd[4] = 0x02;
        cmd[5] = 0x00;
        cmd[6] = 0x00;
        cmd[7] = 0x00;
        cmd[8] = 0x00;
        cmd[9] = 0x00;
        cmd[10] = 0x02;
        cmd[11] = 0x21;
        cmd[12] = 0x7D;
    }

    comContains->write(cmd,13);
    delay(50);
    comContains->write(cmd,13);
}

void serialPPL::containsDC(int index)
{
    char cmd[15];

    if(index == 1)
    {
        cmd[0] = 0x7B;
        cmd[1] = 0x0D;
        cmd[2] = 0x00;
        cmd[3] = 0x10;
        cmd[4] = 0x05;
        cmd[5] = 0x00;
        cmd[6] = 0x00;
        cmd[7] = 0x00;
        cmd[8] = 0x00;
        cmd[9] = 0x00;
        cmd[10] = 0x05;
        cmd[11] = 0x27;
        cmd[12] = 0x7D;
    }else{
        cmd[0] = 0x7B;
        cmd[1] = 0x0D;
        cmd[2] = 0x00;
        cmd[3] = 0x10;
        cmd[4] = 0x04;
        cmd[5] = 0x00;
        cmd[6] = 0x00;
        cmd[7] = 0x00;
        cmd[8] = 0x00;
        cmd[9] = 0x00;
        cmd[10] = 0x04;
        cmd[11] = 0x25;
        cmd[12] = 0x7D;
    }

    comContains->write(cmd,13);
    delay(50);
    comContains->write(cmd,13);
}

void serialPPL::containsIO(int index)
{
    char cmd[15];

    if(index == 1)
    {
        cmd[0] = 0x7B;
        cmd[1] = 0x31;
        cmd[2] = 0x39;
        cmd[3] = 0x38;
        cmd[4] = 0x2C;
        cmd[5] = 0x30;
        cmd[6] = 0x36;
        cmd[7] = 0x30;
        cmd[8] = 0x30;
        cmd[9] = 0x2C;
        cmd[10] = 0x43;
        cmd[11] = 0x30;
        cmd[12] = 0x7D;
    }else{
        cmd[0] = 0x7B;
        cmd[1] = 0x30;
        cmd[2] = 0x30;
        cmd[3] = 0x30;
        cmd[4] = 0x2C;
        cmd[5] = 0x30;
        cmd[6] = 0x36;
        cmd[7] = 0x30;
        cmd[8] = 0x30;
        cmd[9] = 0x2C;
        cmd[10] = 0x41;
        cmd[11] = 0x45;
        cmd[12] = 0x7D;
    }

    comContains->write(cmd,13);
    delay(100);
    comContains->write(cmd,13);
}

int serialPPL::readPowerData(testDataStr *tds)
{
    char cmd[5];
    int i;
    struct timeval tpstart,tpend;
    float timeuse;
    float voltage,current,power,freq,pf;
    QByteArray buffer;

    cmd[0] = 0x55;
    cmd[1] = 0x01;
    cmd[2] = 0x10;
    cmd[3] = 0x66;

    i = comPPL->write(cmd,4);

    if(i < 0)
    {
        delay(100);
        i = comPPL->write(cmd,4);
        if(i < 0)
            return -1;
    }

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 1000.0)
        {
            break;
        }else if(comPPL->bytesAvailable() >= 24){
            buffer = comPPL->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(buffer.size() != 24)
    {
        return -1;
    }

    if(quint8(buffer.at(0)) == 170 && quint8(buffer.at(2)) == 16)
    {        
        un_4b.uc[0] = quint8(buffer.at(3));
        un_4b.uc[1] = quint8(buffer.at(4));
        un_4b.uc[2] = quint8(buffer.at(5));
        un_4b.uc[3] = quint8(buffer.at(6));
        voltage = un_4b.fda;

        un_4b.uc[0] = quint8(buffer.at(7));
        un_4b.uc[1] = quint8(buffer.at(8));
        un_4b.uc[2] = quint8(buffer.at(9));
        un_4b.uc[3] = quint8(buffer.at(10));
        current = un_4b.fda;

        un_4b.uc[0] = quint8(buffer.at(11));
        un_4b.uc[1] = quint8(buffer.at(12));
        un_4b.uc[2] = quint8(buffer.at(13));
        un_4b.uc[3] = quint8(buffer.at(14));
        power = un_4b.fda;

        tds->group = 2;
        tds->step = step;
        tds->id = 38;
        tds->model = 6;
        tds->next = 1;
        tds->testTime = itemTime.at(step) - powerTime/1000.0;
        if(tds->testTime < 0.0)
            tds->testTime = 0.0;

        if(power < itemMin.at(step))
        {
            tds->flag = 0;
            tds->flag1 = 0;
            tds->flag2 = 1;
            tds->flag3 = 0;
            tds->flag4 = 1;
            tds->flag5 = 1;
            tds->flag6 = 1;
            tds->flag7 = 1;
            tds->flag8 = 1;
        }else if(power > itemMax.at(step)){
            tds->flag = 0;
            tds->flag1 = 0;
            tds->flag2 = 1;
            tds->flag3 = 1;
            tds->flag4 = 0;
            tds->flag5 = 1;
            tds->flag6 = 1;
            tds->flag7 = 1;
            tds->flag8 = 1;
        }else{
            tds->flag = 1;
            tds->flag1 = 1;
            tds->flag2 = 0;
            tds->flag3 = 0;
            tds->flag4 = 0;
            tds->flag5 = 0;
            tds->flag6 = 0;
            tds->flag7 = 1;
            tds->flag8 = 1;
        }

        tds->voltage = voltage;
        tds->current = current;
        tds->power = power;
    }

    return 1;
}

void serialPPL::powerTimeRun()
{
    struct timeval tpstart,tpend;
    float timeuse;

    gettimeofday(&tpend,NULL);
    timeuse = (1000000*(tpend.tv_sec-pplStart.tv_sec) + tpend.tv_usec-pplStart.tv_usec)/1000.0;
    powerTime += timeuse;
    qDebug()<<"powerTime:"<<powerTime;

    if(powerTime >= 2000)
    {
        powerDo = 0;
        powerTime = 0.0;
        step++;
        timerPPL->stop();
    }
}

void serialPPL::powerItemsTest()
{
    if(powerRun == 1)
        return;

    stop = 1;
    powerStop = 0;
    powerRun = 1;

    testDataStr tds;
    struct timeval tpstart,tpend;
    float timeuse;
    int num = itemId.size();
    step = 0;

    if(acdc == 0)
        containsAC(1);
    else
        containsDC(1);

    for(int i=0;i<num;i++)
    {
        if(powerStop == 1)
            break;
        step = i;
        powerTime = 0.0;

        gettimeofday(&pplStart,NULL);
        while(true)
        {
            QCoreApplication::processEvents();
            gettimeofday(&tpend,NULL);
            powerTime = (1000000*(tpend.tv_sec-pplStart.tv_sec) + tpend.tv_usec-pplStart.tv_usec)/1000.0;
            if(powerStop == 1)
            {
                bool ok = readPowerData(&tds);
                tds.flag = 1;
                tds.testTime = 0.0;
                if(ok == 1)
                    emit sendCommand(4,1,tds);

                step = 0;
                stop = 0;
                powerRun = 0;
                powerStop = 0;

                if(acdc == 0)
                    containsAC(0);
                else
                    containsDC(0);

                return;
            }
            bool ok = readPowerData(&tds);
            tds.flag = 0;
            if(ok == 1)
                emit sendCommand(3,0,tds);

            if(powerTime >= itemTime.at(i)*1000.0)
            {
                bool ok = readPowerData(&tds);
                tds.flag = 1;
                if(ok == 1)
                {
                    if(i == num-1)
                        emit sendCommand(4,1,tds);
                    else
                        emit sendCommand(4,0,tds);
                }
                break;
            }
            QCoreApplication::processEvents();
        }
        if(powerStop == 1)
            break;
    }
    step = 0;
    stop = 0;
    powerRun = 0;
    powerStop = 0;

    if(acdc == 0)
        containsAC(0);
    else
        containsDC(0);
}

void serialPPL::readStatus()
{
    if(stop == 1)
        return;

    int i,j,k,h;
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

    com->readAll();
    delay(30);

    cmd[0] = 0x7B;
    cmd[1] = 0x06;
    cmd[2] = addr;
    cmd[3] = 0x03;
    cmd[4] = getChecksum(cmd,1,3);
    cmd[5] = 0x7D;

    if(stop == 1)
        return;

    i = com->write(cmd,6);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,6);
    }

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        if(stop == 1)
            return;

        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 200.0)
        {
            break;
        }else if(com->bytesAvailable() >= 9){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(stop == 1)
        return;

    if(buffer.size() == 9 && buffer.at(3) == 0x03)
    {
        j = quint8(buffer.at(4));
        k = quint8(buffer.at(5));
        h = quint8(buffer.at(6));

        if(h == 1 && j != 3)
        {
            if(powerOnly == 1)
            {
                alarmEnabled(1);
                powerItemsTest();
            }else if(powerDo == 0){
                stop = 1;
                startTest(0);
                stop = 0;
            }

            com->readAll();
            delay(30);
            i = com->write(cmd,6);
            gettimeofday(&tpstart,NULL);
            while(true)
            {
                if(stop == 1)
                    return;

                QCoreApplication::processEvents();
                gettimeofday(&tpend,NULL);
                timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

                if(timeuse > 200.0)
                {
                    break;
                }else if(com->bytesAvailable() >= 9){
                    buffer = com->readAll();
                    break;
                }
                QCoreApplication::processEvents();
            }
        }else{
            if(status == 4 && subStatus == 1 && j == 3)
                step = 0;
            error = 0;
            status = j;
            subStatus = k;
            if(j == 3 || j == 4)
            {
                int ok = 0;
                int tb = (step+1>=itemId.size())?step:step+1;
                testDataStr tds;
                if(j == 4 && (step <= itemId.size()-1) && itemId.at(tb) == 6)
                {
                    if(powerDo == 0 && powerStop == 1)
                    {
                        stop = 1;
                        ok = readTestData(4,1,&tds);
                        if(ok == 1)
                            emit sendCommand(4,1,tds);
                        stop = 0;

                        if(acdc == 0)
                            containsAC(0);
                        else
                            containsDC(0);
                        return;
                    }

                    if(powerDo == 0 && itemId.at(step) != 6)
                    {
                        int te = step+1;
                        stop = 1;
                        ok = readTestData(4,0,&tds);
                        if(ok == 1)
                            emit sendCommand(4,0,tds);
                        stop = 0;
                        step = te;
                        powerDo++;
                        powerTime = 0.0;

                        if(acdc == 0)
                            containsAC(1);
                        else
                            containsDC(1);
                        gettimeofday(&pplStart,NULL);
                    }

                    struct timeval tpend;
                    float timeuse;

                    gettimeofday(&tpend,NULL);
                    timeuse = (1000000*(tpend.tv_sec-pplStart.tv_sec) + tpend.tv_usec-pplStart.tv_usec)/1000.0;
                    powerTime = timeuse;

                    if(powerTime >= itemTime.at(step)*1000.0)
                    {
                        if(step < itemId.size()-1)
                        {
                            ok = readPowerData(&tds);
                            tds.flag = 1;
                            if(ok == 1)
                                emit sendCommand(4,0,tds);

                            step++;
                            powerDo = 0;
                            powerTime = 0.0;

                            if(acdc == 0)
                                containsAC(0);
                            else
                                containsDC(0);

                            gettimeofday(&pplStart,NULL);
                        }else{
                            ok = readPowerData(&tds);
                            tds.flag = 1;
                            tds.testTime = 0.0;
                            if(ok == 1)
                                emit sendCommand(4,1,tds);

                            powerDo = 0;
                            powerTime = 0.0;

                            if(acdc == 0)
                                containsAC(0);
                            else
                                containsDC(0);

//                            containsIO(0);
                            stop = 0;
                        }
                    }else{
                        ok = readPowerData(&tds);
                        if(ok == 1)
                        {
                            if(powerStop == 0)
                            {
                                emit sendCommand(3,0,tds);
                            }else{
                                tds.flag = 1;
                                tds.testTime = 0.0;
                                emit sendCommand(4,1,tds);
                                powerDo = 0;
                                powerTime = 0.0;

                                if(acdc == 0)
                                    containsAC(0);
                                else
                                    containsDC(0);
                            }
                        }
                    }
                    stop = 0;
                }else{
                    stop = 1;
                    ok = readTestData(j,k,&tds);
                    if(ok == 1)
                        emit sendCommand(j,k,tds);
                    stop = 0;
                }
            }else{
                step = 0;
                powerTime = 0.0;
                testDataStr tds;
                emit sendCommand(j,k,tds);
            }
        }
    }else{
        qDebug()<<"error"<<buffer.toHex()<<status<<j;

        if(buffer.count() == 0)
            error++;
        if(error > 10)
        {
            error = 0;
            testDataStr tds;
            emit sendCommand(-1,0,tds);
        }
    }
}

void serialPPL::printBinary(int n)
{
    int i = 0;
    char abcd[10];
    strcpy(Binary,"");
    if(n>1)
    {
        printBinary(n/2);
    }
    sprintf(abcd,"%d",n%2);
    strcat(Binary,abcd);
}

int serialPPL::readTestData(int index,int index1,testDataStr *tds)
{
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

    cmd[0] = 0x7B;
    cmd[1] = 0x07;
    cmd[2] = addr;
    cmd[3] = 0x04;
    cmd[4] = step;
    cmd[5] = getChecksum(cmd,1,4);
    cmd[6] = 0x7D;

    int i = com->write(cmd,7);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,7);
    }

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 300.0)
        {
            break;
        }else if(com->bytesAvailable() >= 25){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(buffer.size() != 25)
    {
        return -1;
    }

    if(buffer.at(0) == 0x7B && buffer.at(1) == 0x19 && buffer.at(3) == 0x04)
    {
        tds->group = quint8(buffer.at(5))<<8|quint8(buffer.at(4));
        tds->step = buffer.at(6);
        tds->id = buffer.at(7);
        printBinary(tds->id);
        if(strlen(Binary) < 8)
        {
            char tt[128];
            strcpy(tt,"");
            for(int i=0;i<8-strlen(Binary);i++)
                strcat(tt,"0");
            strcat(tt,Binary);
            strcpy(Binary,tt);
        }

        int sum = 0;
        for(int i=7;i>=4;i--)
        {
            sum += (Binary[i]-'0')*((int)pow(2,7-i));
        }
        tds->model = sum;
        tds->next = Binary[2];
        tds->testTime = (quint8(buffer.at(9))<<8|quint8(buffer.at(8)))/10.0;

        int flag = quint8(buffer.at(10));
        tds->flag = flag;

        if(tds->model == 0)
        {
            step = tds->step+1;
            return -1;
        }

        if(tds->testTime == 0.0 && flag == 0 && index != 4 && index1 != 1)
        {
            return -1;
        }

        if(tds->testTime == 0.0 && flag != 0 && step < tc.step && index != 4 && index1 != 1)
        {
            if(step < tc.itemNum-1 && itemId.at(step+1) != 6)
                step++;
        }

        printBinary(flag);

        if(strlen(Binary) < 8)
        {
            char tt[128];
            strcpy(tt,"");
            for(int i=0;i<8-strlen(Binary);i++)
                strcat(tt,"0");
            strcat(tt,Binary);
            strcpy(Binary,tt);
        }

        QString tt = Binary;
        tds->flag1 = tt.mid(7,1).toInt();
        tds->flag2 = tt.mid(6,1).toInt();
        tds->flag3 = tt.mid(5,1).toInt();
        tds->flag4 = tt.mid(4,1).toInt();
        tds->flag5 = tt.mid(3,1).toInt();
        tds->flag6 = tt.mid(2,1).toInt();
        tds->flag7 = tt.mid(1,1).toInt();
        tds->flag8 = tt.mid(0,1).toInt();

        switch(tds->model)
        {
            case 1:
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/1.0;
                if(tc.ACW_I_K == "1")
                    tds->current = (quint8(buffer.at(16))<<24|quint8(buffer.at(15))<<16|quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1000.0;
                else if(tc.ACW_I_K == "10")
                    tds->current = (quint8(buffer.at(16))<<24|quint8(buffer.at(15))<<16|quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/100.0;
                else
                    tds->current = (quint8(buffer.at(16))<<24|quint8(buffer.at(15))<<16|quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1000.0;
                break;
            case 2:
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/1.0;
                tds->current = (quint8(buffer.at(16))<<24|quint8(buffer.at(15))<<16|quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1000.0;
                break;
            case 3:
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/1.0;
                tds->resistance = (quint8(buffer.at(16))<<24|quint8(buffer.at(15))<<16|quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/100.0;
                break;
            case 4:
                tds->current = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/100.0;
                tds->voltage = (quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1.0;
                tds->resistance = (quint8(buffer.at(16))<<8|quint8(buffer.at(15)))/1.0;
                break;
            case 5:
                {
                    tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/1.0;
                    tds->pCurrent = (quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1.0;

                    tds->current = (quint8(buffer.at(18))<<24|quint8(buffer.at(17))<<16|quint8(buffer.at(16))<<8|quint8(buffer.at(15)))/10000.0;
                    tds->lCurrent = (quint8(buffer.at(22))<<24|quint8(buffer.at(21))<<16|quint8(buffer.at(20))<<8|quint8(buffer.at(19)))/1000.0;
                }
                break;
            case 6:
            case 7:
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/1.0;
                tds->current = (quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/100.0;
                tds->power = (quint8(buffer.at(16))<<8|quint8(buffer.at(15)))/1.0;
                break;
            case 8:
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/10.0;
                tds->resistance = (quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/10.0;
                break;
        }
    }
    return 1;
}

void serialPPL::readGroupData(groupConfig group)
{
    int id,gid,v,min,max,t,m,min1,max1,t1;
    char cmd[29];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer,dd;

    if(group.groupNum < 0 || status == 3)
        return;

    stop = 1;
    if(status == 4)
    {
        stopTest();
    }

    int i = chooseGroup(group.groupNum);
    int md = group.MD;
    int ad = group.ACDC;
    acdc = ad;

    powerOnly = 1;
    itemId.clear();
    itemTime.clear();
    itemMin.clear();
    itemMax.clear();
    for(int i=0;i<tc.itemNumMax;i++)
    {
        delay(50);

        if(i < group.itemNum)
        {
            itemId.append(group.items.at(i).id);
            itemTime.append(group.items.at(i).data[3]*1.0);
            itemMin.append(group.items.at(i).data[1]*1.0);
            itemMax.append(group.items.at(i).data[2]*1.0);

            if(group.items.at(i).id == 6)
            {
                gid = 0;
                QString tt = "00100000";
                char *e;
                dd = tt.toLatin1();
                id = strtol(dd,&e,2);
                v = 0;
                min = 0;
                max = 0;
                t = 0;
            }else{
                powerOnly = 0;
                gid = group.items.at(i).id;
                printBinary(group.items.at(i).id);
                if(strlen(Binary) < 4)
                {
                    char tt[128];
                    strcpy(tt,"");
                    for(int i=0;i<4-strlen(Binary);i++)
                        strcat(tt,"0");
                    strcat(tt,Binary);
                    strcpy(Binary,tt);
                }
                QString tt = "0010"+(QString)Binary;
                char *e;
                dd = tt.toLatin1();
                id = strtol(dd,&e,2);

                switch(gid)
                {
                    case 0:
                    {
                        min = 0;
                        max = 0;
                        v = 0;
                    }
                        break;
                    case 1:
                    {
                        QString a = QString("%1").arg(group.items.at(i).data[1]*100.0);
                        min = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[2]*100.0);
                        max = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[0]*1.0);
                        v = a.toInt();
                    }
                        break;
                    case 2:
                    {
                        QString a = QString("%1").arg(group.items.at(i).data[1]*1000.0);
                        min = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[2]*1000.0);
                        max = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[0]*1.0);
                        v = a.toInt();
                    }
                        break;
                    case 3:
                    {
                        QString a = NULL;
                        if(group.items.at(i).data[1] >= 0.0 && group.items.at(i).data[1] <= 9.99)
                        {
                            a = QString("%1").arg(group.items.at(i).data[1]*100.0);
                            min = a.toInt();
                        } else if(group.items.at(i).data[1] > 9.99 && group.items.at(i).data[1] <= 99.99){
                            a = QString("%1").arg(group.items.at(i).data[1]*10.0);
                            min = a.toInt()+10000;
                        }else{
                            a = QString("%1").arg(group.items.at(i).data[1]*1.0);
                            min = a.toInt()+20000;
                        }

                        if(group.items.at(i).data[2] >= 0.0 && group.items.at(i).data[2] <= 9.99)
                        {
                            a = QString("%1").arg(group.items.at(i).data[2]*100.0);
                            max = a.toInt();
                        }else if(group.items.at(i).data[2] > 9.99 && group.items.at(i).data[2] <= 99.99){
                            a = QString("%1").arg(group.items.at(i).data[2]*10.0);
                            max = a.toInt()+10000;
                        }else{
                            a = QString("%1").arg(group.items.at(i).data[2]*1.0);
                            max = a.toInt()+20000;
                        }

                        a = QString("%1").arg(group.items.at(i).data[0]*1.0);
                        v = a.toInt();
                    }
                        break;
                    case 4:
                    {
                        QString a = QString("%1").arg(group.items.at(i).data[1]*1.0);
                        min = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[2]*1.0);
                        max = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[0]*10.0);
                        v = a.toInt();
                    }
                        break;
                    case 5:
                    {
                        QString a = QString("%1").arg(group.items.at(i).data[1]*1000.0);
                        min = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[2]*1000.0);
                        max = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[0]*1.0);
                        v = a.toInt();
                    }
                        break;
                    case 6:
                    {
                        QString a = QString("%1").arg(group.items.at(i).data[1]*1.0);
                        min = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[2]*1.0);
                        max = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[0]*1.0);
                        v = a.toInt();
                    }
                        break;
                    case 7:
                    {
                        QString a = QString("%1").arg(group.items.at(i).data[1]*100.0);
                        min = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[2]*100.0);
                        max = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[0]*1.0);
                        v = a.toInt();
                    }
                        break;
                    case 8:
                    {
                        QString a = QString("%1").arg(group.items.at(i).data[1]*10.0);
                        min = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[2]*10.0);
                        max = a.toInt();
                        a = QString("%1").arg(group.items.at(i).data[0]*10.0);
                        v = a.toInt();
                    }
                        break;
                }

                t = (int)(group.items.at(i).data[3]*10.0);
                m = group.items.at(i).state;
            }
        }else{
            gid = 0;
            QString tt = "00100000";
            char *e;
            dd = tt.toLatin1();
            id = strtol(dd,&e,2);
            v = 0;
            min = 0;
            max = 0;
            t = 0;
        }

        int g;
        if(group.groupNum == 0)
            g = group.groupNum;
        else
            g = group.groupNum-1;

        cmd[0] = 0x7B;
        cmd[1] = 0x1B;
        cmd[2] = addr;
        cmd[3] = 0x10;
        cmd[4] = g&0xff;
        cmd[5] = g>>8;
        cmd[6] = i;
        cmd[7] = id;
        cmd[8] = 0x00;
        switch(gid)
        {
            case 0:
                cmd[9] = v&0xff;
                cmd[10] = v>>8;
                cmd[11] = max&0xff;
                cmd[12] = max>>8;
                cmd[13] = min&0xff;
                cmd[14] = min>>8;
                cmd[15] = t&0xff;
                cmd[16] = t>>8;
                cmd[17] = 0x00;
                cmd[18] = 0x00;
                cmd[19] = 0x00;
                cmd[20] = 0x00;
                cmd[21] = 0x00;
                cmd[22] = 0x00;
                cmd[23] = 0x00;
                cmd[24] = 0x00;
                break;
            case 1:
                cmd[9] = v&0xff;
                cmd[10] = v>>8;
                cmd[11] = max&0xff;
                cmd[12] = max>>8;
                cmd[13] = min&0xff;
                cmd[14] = min>>8;
                cmd[15] = t&0xff;
                cmd[16] = t>>8;
                cmd[17] = 0x00;
                cmd[18] = 0x00;
                cmd[19] = 0x00;
                cmd[20] = 0x00;
                cmd[21] = 0x00;
                cmd[22] = 0x00;
                cmd[23] = 0x00;
                cmd[24] = 0x00;
                break;
            case 2:
                cmd[9] = v&0xff;
                cmd[10] = v>>8;
                cmd[11] = max&0xff;
                cmd[12] = max>>8;
                cmd[13] = min&0xff;
                cmd[14] = min>>8;
                cmd[15] = t&0xff;
                cmd[16] = t>>8;
                cmd[17] = 0x00;
                cmd[18] = 0x00;
                cmd[19] = 0x00;
                cmd[20] = 0x00;
                cmd[21] = 0x00;
                cmd[22] = 0x00;
                cmd[23] = 0x00;
                cmd[24] = 0x00;
                break;
            case 3:
                cmd[9] = v&0xff;
                cmd[10] = v>>8;
                cmd[11] = max&0xff;
                cmd[12] = max>>8;
                cmd[13] = min&0xff;
                cmd[14] = min>>8;
                cmd[15] = t&0xff;
                cmd[16] = t>>8;
                cmd[17] = 0x00;
                cmd[18] = 0x00;
                cmd[19] = 0x00;
                cmd[20] = 0x00;
                cmd[21] = 0x00;
                cmd[22] = 0x00;
                cmd[23] = 0x00;
                cmd[24] = 0x00;
                break;
            case 4:
                if(group.gnd == 0)
                {
                    cmd[9] = v&0xff;
                    cmd[10] = v>>8;
                    cmd[11] = 300&0xff;
                    cmd[12] = 300>>8;
                    cmd[13] = 30&0xff;
                    cmd[14] = 30>>8;
                    cmd[15] = t&0xff;
                    cmd[16] = t>>8;
                    cmd[17] = 0x00;
                    cmd[18] = 0x00;
                    cmd[19] = max&0xff;
                    cmd[20] = max>>8;
                    cmd[21] = min&0xff;
                    cmd[22] = min>>8;
                    cmd[23] = 0x00;
                    cmd[24] = 0x00;
                }else{
                    cmd[9] = v&0xff;
                    cmd[10] = v>>8;
                    cmd[11] = max&0xff;
                    cmd[12] = max>>8;
                    cmd[13] = min&0xff;
                    cmd[14] = min>>8;
                    cmd[15] = t&0xff;
                    cmd[16] = t>>8;
                    cmd[17] = 0x00;
                    cmd[18] = 0x00;
                    cmd[19] = 100&0xff;
                    cmd[20] = 100>>8;
                    cmd[21] = 10&0xff;
                    cmd[22] = 10>>8;
                    cmd[23] = 0x00;
                    cmd[24] = 0x00;
                }
                break;
            case 5:
                cmd[9] = v&0xff;
                cmd[10] = v>>8;
                cmd[11] = 0x00;
                cmd[12] = 0x00;
                cmd[13] = max&0xff;
                cmd[14] = max>>8;
                cmd[15] = min&0xff;
                cmd[16] = min>>8;
                cmd[17] = t&0xff;
                cmd[18] = t>>8;
                if(tc.md == 1)
                {
                    cmd[19] = md&0xff;
                    cmd[20] = md>>8;
                }else{
                    cmd[19] = 0x00;
                    cmd[20] = 0x00;
                }
                cmd[21] = m&0xff;
                cmd[22] = m>>8;
                cmd[23] = 0x00;
                cmd[24] = 0x00;
                break;
            case 6:
                cmd[9] = 0x00;
                cmd[10] = 0x00;
                cmd[11] = 0x00;
                cmd[12] = 0x00;
                cmd[13] = max&0xff;
                cmd[14] = max>>8;
                cmd[15] = min&0xff;
                cmd[16] = min>>8;
                cmd[17] = t&0xff;
                cmd[18] = t>>8;
                cmd[19] = 0x00;
                cmd[20] = 0x00;
                cmd[21] = v&0xff;
                cmd[22] = v>>8;
                cmd[23] = 0x00;
                cmd[24] = 0x00;
                break;
            case 7:
                cmd[9] = max&0xff;
                cmd[10] = max>>8;
                cmd[11] = min&0xff;
                cmd[12] = min>>8;
                cmd[13] = 0x00;
                cmd[14] = 0x00;
                cmd[15] = 0x00;
                cmd[16] = 0x00;
                cmd[17] = t&0xff;
                cmd[18] = t>>8;
                cmd[19] = 0x00;
                cmd[20] = 0x00;
                cmd[21] = v&0xff;
                cmd[22] = v>>8;
                cmd[23] = 0x00;
                cmd[24] = 0x00;
                break;
        }
        cmd[25] = getChecksum(cmd,1,24);
        cmd[26] = 0x7D;

        int h = com->write(cmd,27);

        if(h < 0)
        {
            delay(100);
            h = com->write(cmd,27);
        }

        if(h < 0)
            continue;
    }


    com->readAll();
    delay(100);

    if(tc.phase == 1 || tc.phase == 2)
    {
        fillCommonPhase(group);
    }else{
        fillCommon(group);
    }

    if(ad == 0)
    {
        containsAC(0);
    }else{
        containsDC(0);
    }
    stop = 0;
}

int serialPPL::readMachineStatus()
{
    int i,j,k,h;
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;
    QStringList msg;

    cmd[0] = 0x7B;
    cmd[1] = 0x06;
    cmd[2] = addr;
    cmd[3] = 0x03;
    cmd[4] = getChecksum(cmd,1,3);
    cmd[5] = 0x7D;

    i = com->write(cmd,6);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,6);
    }

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 1000.0)
        {
            break;
        }else if(com->bytesAvailable() >= 9){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }
    j = -1;
    if(buffer.size() == 9 && buffer.at(3) == 0x03)
    {
        j = quint8(buffer.at(4));
        k = quint8(buffer.at(5));
        h = quint8(buffer.at(6));
    }
    return j;
}

void serialPPL::waitStateFun()
{
    int sum = 0;
    while(true)
    {
        startTest(0);
        int s = readMachineStatus();
        sum++;
        if(sum > 10 || s == 3)
            break;
    }
}

void serialPPL::serialReadMessage(quint16 addr,quint16 cmd,QByteArray msg)
{
    if(cmd == 1 && addr == 0)
    {
        openSerial();
    }else if(cmd == 1 && addr == 1){
        closeSerial();
    }

    if(addr == 0 && cmd == 0 && msg == "startTest")
    {
        stop = 1;
        startTest(0);
        stop = 0;
    }

    if(addr == 0 && cmd == 0 && msg == "stopTest")
    {
        stop = 1;
        stopTest();
        stop = 0;
    }

    if(addr == 2  && stop == 0)
    {
        stop = 1;
        sendSysConfig(msg);
        stop = 0;
    }
}

int serialPPL::fillCommonPhase(groupConfig group)
{
    char cmd[32];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer,name,dd;

    com->readAll();
    QString tt = QString("000000000000%1%2%3%4")
            .arg(group.invalid).arg(group.NG).arg(group.gnd).arg(group.frequency);
    char *e;
    dd = tt.toLatin1();
    int id = strtol(dd,&e,2);

    QString a = QString("%1").arg(group.upTime*10.0);
    int up = a.toInt();
    a = QString("%1").arg(group.downTime*10.0);
    int dw = a.toInt();

    int phase = 0;
    if(group.phase > 0 && group.phase < 3)
        phase = group.phase;

    cmd[0] = 0x7B;
    cmd[1] = 0x1B;
    cmd[2] = addr;
    cmd[3] = 0x14;
    cmd[4] = 0x61;
    cmd[5] = 0x62;
    cmd[6] = 0x63;
    cmd[7] = 0x64;
    cmd[8] = 0x65;
    cmd[9] = 0x66;
    cmd[10] = 0x67;
    cmd[11] = 0x68;
    cmd[12] = 0x69;
    cmd[13] = 0x6A;
    cmd[14] = 0x6B;
    cmd[15] = 0x6C;
    cmd[16] = 0x6D;
    cmd[17] = 0x6E;
    cmd[18] = id&0xff;
    cmd[19] = id>>8;
    cmd[20] = up&0xff;
    cmd[21] = up>>8;
    cmd[22] = dw&0xff;
    cmd[23] = dw>>8;
    cmd[24] = phase;
    cmd[25] = getChecksum(cmd,1,24);
    cmd[26] = 0x7D;

    int i = com->write(cmd,27);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,27);
    }

    if(i < 0)
        return -1;

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 2000.0)
        {
            break;
        }else if(com->bytesAvailable() >= 6){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(buffer.toHex() == "7b060001077d")
        return 1;
    else
        return -1;
}

int serialPPL::fillCommon(groupConfig group)
{
    char cmd[32];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer,name,dd;

    com->readAll();

    QString tt = QString("000000000000%1%2%3%4")
            .arg(group.invalid).arg(group.NG).arg(group.gnd).arg(group.frequency);
    char *e;
    dd = tt.toLatin1();
    int id = strtol(dd,&e,2);

    QString a = QString("%1").arg(group.upTime*10.0);
    int up = a.toInt();
    a = QString("%1").arg(group.downTime*10.0);
    int dw = a.toInt();

    cmd[0] = 0x7B;
    cmd[1] = 0x1A;
    cmd[2] = addr;
    cmd[3] = 0x14;
    cmd[4] = 0x61;
    cmd[5] = 0x62;
    cmd[6] = 0x63;
    cmd[7] = 0x64;
    cmd[8] = 0x65;
    cmd[9] = 0x66;
    cmd[10] = 0x67;
    cmd[11] = 0x68;
    cmd[12] = 0x69;
    cmd[13] = 0x6A;
    cmd[14] = 0x6B;
    cmd[15] = 0x6C;
    cmd[16] = 0x6D;
    cmd[17] = 0x6E;
    cmd[18] = id&0xff;
    cmd[19] = id>>8;
    cmd[20] = up&0xff;
    cmd[21] = up>>8;
    cmd[22] = dw&0xff;
    cmd[23] = dw>>8;
    cmd[24] = getChecksum(cmd,1,23);
    cmd[25] = 0x7D;

    int i = com->write(cmd,26);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,26);
    }

    if(i < 0)
        return -1;

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 2000.0)
        {
            break;
        }else if(com->bytesAvailable() >= 6){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(buffer.toHex() == "7b060001077d")
        return 1;
    else
        return -1;
}

void serialPPL::alarmSlots(int index)
{
    alarmEnabled(index);
}

void serialPPL::alarmEnabled(int index)
{
    int i = 0;
    char cmd[8];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer,name,dd;

    com->readAll();

    if(index == 2)
    {
        cmd[0] = 0x7B;
        cmd[1] = 0x07;
        cmd[2] = 0x00;
        cmd[3] = 0x17;
        cmd[4] = 0x0A;
        cmd[5] = getChecksum(cmd,1,4);
        cmd[6] = 0x7D;

        i = com->write(cmd,7);
        delay(100);
        i = com->write(cmd,7);
        delay(1000);

        cmd[0] = 0x7B;
        cmd[1] = 0x07;
        cmd[2] = 0x00;
        cmd[3] = 0x17;
        cmd[4] = 0x02;
        cmd[5] = getChecksum(cmd,1,4);
        cmd[6] = 0x7D;

        i = com->write(cmd,7);
        delay(100);
        i = com->write(cmd,7);
    }else if(index == 4){
        cmd[0] = 0x7B;
        cmd[1] = 0x07;
        cmd[2] = 0x00;
        cmd[3] = 0x17;
        cmd[4] = 0x0C;
        cmd[5] = getChecksum(cmd,1,4);
        cmd[6] = 0x7D;

        i = com->write(cmd,7);
        delay(100);
        i = com->write(cmd,7);
    }else{
        cmd[0] = 0x7B;
        cmd[1] = 0x07;
        cmd[2] = 0x00;
        cmd[3] = 0x17;
        cmd[4] = index;
        cmd[5] = getChecksum(cmd,1,4);
        cmd[6] = 0x7D;

        i = com->write(cmd,7);
        delay(100);
        i = com->write(cmd,7);
    }
}

void serialPPL::sendSysConfig(QByteArray msg)
{
    if(msg.size() != 16)
        return;

    char cmd[8];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;
    char *e;
    int id = strtol(msg,&e,2);

    com->readAll();

    cmd[0] = 0x7B;
    cmd[1] = 0x08;
    cmd[2] = addr;
    cmd[3] = 0x15;
    cmd[4] = id&0xff;
    cmd[5] = id>>8;
    cmd[6] = getChecksum(cmd,1,5);
    cmd[7] = 0x7D;

    int i = com->write(cmd,8);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,8);
    }

    if(i < 0)
        return;

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 1000.0)
        {
            break;
        }else if(com->bytesAvailable() >= 6){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(buffer.toHex() == "7b060001077d")
        return;
    else{
        buffer.clear();
        delay(100);
        i = com->write(cmd,8);

        gettimeofday(&tpstart,NULL);
        while(true)
        {
            QCoreApplication::processEvents();
            gettimeofday(&tpend,NULL);
            timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

            if(timeuse > 1000.0)
            {
                break;
            }else if(com->bytesAvailable() >= 6){
                buffer = com->readAll();
                break;
            }
            QCoreApplication::processEvents();
        }
    }
}

int serialPPL::chooseGroup(int index)
{
    if(index < 0)
        return -1;

    char cmd[8];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;
    int i;

    if(index > 0)
        i = index-1;
    else
        i = 0;

    delay(100);
    com->readAll();    

    cmd[0] = 0x7B;
    cmd[1] = 0x08;
    cmd[2] = addr;
    cmd[3] = 0x0B;
    cmd[4] = i&0xff;
    cmd[5] = i>>8;
    cmd[6] = getChecksum(cmd,1,5);
    cmd[7] = 0x7D;

    i = com->write(cmd,8);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,8);
    }

    if(i < 0)
        return -1;

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 1000.0)
        {
            break;
        }else if(com->bytesAvailable() >= 6){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(buffer.toHex() == "7b060001077d")    //下位机正确执行
        return 1;
    else{
        com->write(cmd,8);
        return -1;
    }
}

void serialPPL::readGroupSetData(int g,int s)
{
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

    com->readAll();
    delay(100);

    cmd[0] = 0x7B;
    cmd[1] = 0x09;
    cmd[2] = addr;
    cmd[3] = 0x05;
    cmd[4] = g&0xff;
    cmd[5] = g>>8;
    cmd[6] = s;
    cmd[7] = getChecksum(cmd,1,6);
    cmd[8] = 0x7D;

    int i = com->write(cmd,9);

    if(i < 0)
    {
        delay(100);
        i = com->write(cmd,9);
    }

    if(i < 0)
        return;

    gettimeofday(&tpstart,NULL);
    while(true)
    {
        QCoreApplication::processEvents();
        gettimeofday(&tpend,NULL);
        timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

        if(timeuse > 2000.0)
        {
            break;
        }else if(com->bytesAvailable() >= 6){
            buffer = com->readAll();
            break;
        }
        QCoreApplication::processEvents();
    }

    if(buffer.toHex() == "7b060001077d")
    {
        return;
    }else{
        i = com->write(cmd,7);
        return;
    }
}

int serialPPL::startTest(int index)
{
    if(powerDo == 1)
        return -1;

    powerStop = 0;
    step = 0;
    alarmEnabled(1);
    delay(100);
    if(powerOnly == 0)
    {
        char cmd[7];
        struct timeval tpstart,tpend;
        float timeuse;
        QByteArray buffer;

        com->readAll();
        delay(100);

        cmd[0] = 0x7B;
        cmd[1] = 0x07;
        cmd[2] = addr;
        cmd[3] = 0x01;
        cmd[4] = index;
        cmd[5] = getChecksum(cmd,1,4);
        cmd[6] = 0x7D;

        int i = com->write(cmd,7);

        if(i < 0)
        {
            delay(100);
            i = com->write(cmd,7);
        }

        if(i < 0)
            return -1;

        gettimeofday(&tpstart,NULL);
        while(true)
        {
            QCoreApplication::processEvents();
            gettimeofday(&tpend,NULL);
            timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;

            if(timeuse > 2000.0)
            {
                break;
            }else if(com->bytesAvailable() >= 6){
                buffer = com->readAll();
                break;
            }
            QCoreApplication::processEvents();
        }

        if(buffer.toHex() == "7b060001077d")
        {
            return 1;
        }else{
            i = com->write(cmd,7);
            return -1;
        }
    }else{
        powerItemsTest();
    }
}

int serialPPL::stopTest()
{
    if(powerDo == 1)
    {
        testDataStr tds;
        bool ok = readPowerData(&tds);
        tds.flag = 1;
        tds.testTime = 0.0;
        if(ok == 1)
            emit sendCommand(4,1,tds);

        powerTime = 0.0;

        if(acdc == 0)
            containsAC(0);
        else
            containsDC(0);

        stop = 0;
    }

    powerDo = 0;
    powerStop = 1;
    alarmEnabled(0);
    if(powerOnly == 0)
    {        
        char cmd[7];
        struct timeval tpstart,tpend;
        float timeuse;
        QByteArray buffer;

        com->readAll();
        delay(100);

        cmd[0] = 0x7B;
        cmd[1] = 0x06;
        cmd[2] = addr;
        cmd[3] = 0x02;
        cmd[4] = getChecksum(cmd,1,3);
        cmd[5] = 0x7D;

        int i = com->write(cmd,6);

        if(i < 0)
        {
            delay(100);
            i = com->write(cmd,6);
        }

        if(i < 0)
            return -1;

        gettimeofday(&tpstart,NULL);
        while(true)
        {
            QCoreApplication::processEvents();
            gettimeofday(&tpend,NULL);
            timeuse = (1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000.0;
            if(timeuse > 2000.0)
            {
                break;
            }else if(com->bytesAvailable() >= 6){
                buffer = com->readAll();
                break;
            }
            QCoreApplication::processEvents();
        }

        if(buffer.toHex() == "7b060001077d")    //下位机正确执行
        {
            return 1;
        }else{
            //        i = com->write(cmd,6);
            return -1;
        }
    }
}
