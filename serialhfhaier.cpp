#include "serialhfhaier.h"

serialHFHaier::serialHFHaier(QObject *parent) : QObject(parent)
{
    addr = tc.SPS.machineAddr;
    baudRate = tc.SPS.baudRate;
    port = tc.SPS.port;
    com = NULL;
    stop = 0;
    step = 0;
    status = -1;
    subStatus = -1;
    error = 0;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readStatus()));
}

void serialHFHaier::openSerial()
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

    timer->start(100);   //100毫秒
}

void serialHFHaier::closeSerial()
{
    timer->stop();
    if(com != NULL && com->isOpen())
        com->close();
    tc.portOpen = 0;
}

char serialHFHaier::getChecksum(char *info,int start,int bytes)
{
    int i;
    int checksum = 0;

    for(i=start;i<(bytes+start);i++)
    {
        checksum += info[i];
    }
    return checksum&0xff;
}

void serialHFHaier::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents();
    }
}

void serialHFHaier::readStatus()
{
    if(stop == 1)
        return;

    int i,j,k,h;
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

    delay(30);
    com->readAll();

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

        if(status == 4 && subStatus == 1 && j == 3)
            step = 0;
        error = 0;
        status = j;
        subStatus = k;
        if(j == 3 || j == 4)
        {
            stop = 1;
            testDataStr tds;
            int ok = readTestData(j,k,&tds);
            if(ok == 1)
                emit sendCommand(j,k,tds);
            stop = 0;
        }else{
            step = 0;
            testDataStr tds;
            emit sendCommand(j,k,tds);
        }
    }else{
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

void serialHFHaier::printBinary(int n)
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

int serialHFHaier::readTestData(int index,int index1,testDataStr *tds)
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
            if(step < tc.itemNum-1)
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
            case 2:            
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/1.0;
                if(tc.ACW_I_K == "1")
                    tds->current = (quint8(buffer.at(16))<<24|quint8(buffer.at(15))<<16|quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1000.0;
                else if(tc.ACW_I_K == "10")
                    tds->current = (quint8(buffer.at(16))<<24|quint8(buffer.at(15))<<16|quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/100.0;
                else
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
                    tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/10.0;
                    tds->pCurrent = (quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1.0;

                    tds->current = (quint8(buffer.at(18))<<24|quint8(buffer.at(17))<<16|quint8(buffer.at(16))<<8|quint8(buffer.at(15)))/10000.0;
                    tds->lCurrent = (quint8(buffer.at(22))<<24|quint8(buffer.at(21))<<16|quint8(buffer.at(20))<<8|quint8(buffer.at(19)))/1000.0;
                }
                break;
            case 6:
            case 7:
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/10.0;
                tds->current = (quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/1000.0;
                tds->power = (quint8(buffer.at(16))<<8|quint8(buffer.at(15)))/10.0;
                break;
            case 8:
                tds->voltage = (quint8(buffer.at(12))<<8|quint8(buffer.at(11)))/10.0;
                tds->resistance = (quint8(buffer.at(14))<<8|quint8(buffer.at(13)))/10.0;
                break;
        }
    }
    return 1;
}

void serialHFHaier::readGroupData(groupConfig group)
{
    int id,gid,v,min,max,t,m,min1,max1,t1;
    char cmd[29];
    struct timeval tpstart,tpend;
    float timeuse;   
    QByteArray buffer,dd;

    if(group.groupNum < 0 || status == 3)
        return;

    stop = 1;
    if(status == 4 || status == -1)
    {
        stopTest();
    }

    int i = chooseGroup(group.groupNum);
    int md = group.MD;    

    for(int i=0;i<tc.itemNumMax;i++)
    {
//        delay(50);

        if(i < group.itemNum)
        {
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
                    QString a = QString("%1").arg(group.items.at(i).data[1]*1000.0);
                    min = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[2]*1000.0);
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
                    a = QString("%1").arg(group.items.at(i).data[4]*10.0);
                    t1 = a.toInt();
                }
                    break;
                case 4:
                {
                    QString a = QString("%1").arg(group.items.at(i).data[1]*1.0);
                    min = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[2]*1.0);
                    max = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[0]*100.0);
                    v = a.toInt();
                }
                    break;
                case 5:
                {
                    QString a = QString("%1").arg(group.items.at(i).data[1]*1000.0);
                    min = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[2]*1000.0);
                    max = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[0]*10.0);
                    v = a.toInt();
                }
                    break;
                case 6:
                {
                    QString a = QString("%1").arg(group.items.at(i).data[1]*1.0);
                    min = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[2]*1.0);
                    max = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[0]*10.0);
                    v = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[4]*1000.0);
                    min1 = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[5]*1000.0);
                    max1 = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[6]*10.0);
                    t1 = a.toInt();
                }
                    break;
                case 7:
                {
                    QString a = QString("%1").arg(group.items.at(i).data[1]*1000.0);
                    min = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[2]*1000.0);
                    max = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[0]*10.0);
                    v = a.toInt();
                    a = QString("%1").arg(group.items.at(i).data[4]*10.0);
                    t1 = a.toInt();
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

            QString a = QString("%1").arg(group.items.at(i).data[3]*10.0);
            t = a.toInt();
            m = group.items.at(i).state;
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
                if(tc.machineModel == 1)
                {
                    cmd[17] = t1&0xff;
                    cmd[18] = t1>>8;
                }else{
                    cmd[17] = 0x00;
                    cmd[18] = 0x00;
                }
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
                if(tc.machineModel == 1)
                {
                    cmd[9] = max1&0xff;
                    cmd[10] = max1>>8;
                    cmd[11] = min1&0xff;
                    cmd[12] = min1>>8;
                }else{
                    cmd[9] = 0x00;
                    cmd[10] = 0x00;
                    cmd[11] = 0x00;
                    cmd[12] = 0x00;
                }
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
                if(tc.machineModel == 1)
                {
                    cmd[23] = t1&0xff;
                    cmd[24] = t1>>8;
                }else{
                    cmd[23] = 0x00;
                    cmd[24] = 0x00;
                }
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
                if(tc.machineModel == 1)
                {
                    cmd[23] = t1&0xff;
                    cmd[24] = t1>>8;
                }else{
                    cmd[23] = 0x00;
                    cmd[24] = 0x00;
                }
                break;
            case 8:
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

//        if(h < 0)
//        {
//            delay(100);
//            h = com->write(cmd,27);
//        }

        QByteArray buffer1;
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
                buffer1 = com->readAll();
                break;
            }
            QCoreApplication::processEvents();
        }

        if(buffer1.toHex() != "7b060001077d")
        {
            delay(50);
            h = com->write(cmd,27);

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
                    buffer1 = com->readAll();
                    break;
                }
                QCoreApplication::processEvents();
            }

            if(buffer1.toHex() != "7b060001077d")
                emit sendItemError();
        }
    }

    com->readAll();
    delay(100);
    if(tc.phase == 1 || tc.phase == 2)
    {
        fillCommonPhase(group);
    }else{
        fillCommon(group);
    }

    stop = 0;
}

int serialHFHaier::readMachineStatus()
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

void serialHFHaier::waitStateFun()
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

void serialHFHaier::serialReadMessage(quint16 addr,quint16 cmd,QByteArray msg)
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

int serialHFHaier::fillCommonPhase(groupConfig group)
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

int serialHFHaier::fillCommon(groupConfig group)
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

void serialHFHaier::sendSysConfig(QByteArray msg)
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

    if(buffer.toHex() == "7b060001077d")    //下位机正确执行
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

int serialHFHaier::chooseGroup(int index)
{
    if(index < 0)
        return -1;

    int i;
    char cmd[8];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

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

//    if(i < 0)
//    {
//        delay(100);
//        i = com->write(cmd,8);
//    }

//    if(i < 0)
//        return -1;

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

    if(buffer.toHex() == "7b060001077d")        //下位机正确执行
    {
        return 1;
    }else{
        struct timeval tpstart1,tpend1;
        float timeuse1;
        QByteArray buffer1;

        com->write(cmd,8);

        gettimeofday(&tpstart1,NULL);
        while(true)
        {
            QCoreApplication::processEvents();
            gettimeofday(&tpend1,NULL);
            timeuse1 = (1000000*(tpend1.tv_sec-tpstart1.tv_sec) + tpend1.tv_usec-tpstart1.tv_usec)/1000.0;

            if(timeuse1 > 2000.0)
            {
                break;
            }else if(com->bytesAvailable() >= 6){
                buffer1 = com->readAll();
                break;
            }
            QCoreApplication::processEvents();
        }

        if(buffer1.toHex() != "7b060001077d")
        {
            return -1;
        }
    }
}

void serialHFHaier::readGroupSetData(int g,int s)
{
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

    delay(100);
    com->readAll();    

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

    if(buffer.toHex() == "7b060001077d")    //下位机正确执行
    {
        return;
    }else{
        i = com->write(cmd,7);
        return;
    }
}

int serialHFHaier::startTest(int index)
{
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

    delay(100);
    com->readAll();    

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
}

int serialHFHaier::stopTest()
{
    char cmd[7];
    struct timeval tpstart,tpend;
    float timeuse;
    QByteArray buffer;

    delay(100);
    com->readAll();    

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
