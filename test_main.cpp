#include "test_main.h"

int canTest;
testConfig tc;
range_Item rangeSets[8];
QStringList itemUnits[8];
QSerialPort *w_port;
QSerialPort *s_port;
int openType;

MainWindow::MainWindow(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    int i = checkFileExists();

    sDo = -1;
    saveDb = 0;
    PASS = -1;
    testStart = 0;
    testing = 0;
    canDo = 0;
    canTest = 99;
    tc.state = -1;
    remind = 0;
    selfOffset = -1;
    psCanDo = -1;
    currentMax = 0.0;
    currentMax1 = 0.0;
    xlFlag = -1;
    iniFlag = -1;
    psDataXSB = 0.0;
    chart = NULL;
    set0 = NULL;
    set1 = NULL;
    series = NULL;
    axis = NULL;
    thread_sp = NULL;
    thread_sql = NULL;
    thread_sca = NULL;
    thread_WeiHuangE32 = NULL;
    thread_df = NULL;
    scannerPort = NULL;
    socket = NULL;
    dustFiltr = NULL;
    testItem_XSB = NULL;
    info = NULL;
    barCodeScan = "";

    readRangeFromDB(rangeSets);
    testIni();

    QSplitterHandle *splitterHandle = ui->splitter->handle(1);
    if(splitterHandle)
    {
        splitterHandle->setDisabled(true);
    }
    ui->lineEdit->setFocus();

    if(regState > 0)
    {
        connect(ui->actionReport,SIGNAL(triggered()),this,SLOT(reportTrigger()));
        connect(ui->actionSystem,SIGNAL(triggered()),this,SLOT(configTrigger()));
        connect(ui->actionClear,SIGNAL(triggered()),this,SLOT(emptyPassFail()));
        connect(ui->actionGroup,SIGNAL(triggered()),this,SLOT(chooseTrigger()));
        connect(ui->actionSelfCheck,SIGNAL(triggered()),this,SLOT(selfCheck()));
        connect(ui->actionGroup_2,SIGNAL(triggered()),this,SLOT(groupTrigger()));
//        connect(ui->actionRange,SIGNAL(triggered()),this,SLOT(rangeTrigger()));
//        connect(ui->actionSenior,SIGNAL(triggered()),this,SLOT(seniorTrigger()));
//        connect(ui->actionRole,SIGNAL(triggered()),this,SLOT(roleTrigger()));
//        connect(ui->actionFile,SIGNAL(triggered()),this,SLOT(fileTrigger()));
//        connect(ui->actionUser,SIGNAL(triggered()),this,SLOT(userTrigger()));
        connect(ui->actionTest,SIGNAL(triggered()),this,SLOT(testStartFun()));
        connect(ui->actionStop,SIGNAL(triggered()),this,SLOT(stopTest()));
        connect(ui->toolButton,SIGNAL(clicked(bool)),this,SLOT(testStartFun()));
        connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(stopTest()));
        connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(getBarCode()));
    }
    connect(ui->actionRegister,SIGNAL(triggered()),this,SLOT(registerTrigger()));   
}

MainWindow::~MainWindow()
{
    if(regState > 0)
    {
        if(thread_sca != NULL)
            sendStop();
        thread_sp->quit();
        thread_sp->wait();
        thread_sql->quit();
        thread_sql->wait();
        if(thread_WeiHuangE32 != NULL)
        {
            qDebug()<<"quitSerialWeiHuangE32";
            thread_WeiHuangE32->quit();
            thread_WeiHuangE32->wait();
        }
        if(thread_sca != NULL)
        {
            thread_sca->quit();
            thread_sca->wait();
        }

        if(thread_df != NULL)
        {
            thread_df->quit();
            thread_df->wait();
        }

        delete thread_sp;
        delete thread_sql;
        if(thread_WeiHuangE32 != NULL)
        {
            qDebug()<<"deleteSerialWeiHuangE32";
            delete thread_WeiHuangE32;
        }
        if(thread_sca != NULL)
            delete thread_sca;

        if(thread_df != NULL)
            delete thread_df;

        if(socket != NULL)
        {
            tcpTimer->stop();
            delete tcpTimer;
            delete socket;
        }
    }
    writeIniFile();
    delete ui;
}

void MainWindow::initThreadAll()
{
    initSql();
    initSerial();
    if((tc.PS.autoPrint == 1) && (tc.PS.PrintType == 1))
    {
        qDebug()<<"initSerialWeiHuangE32";
        initSerialWeiHuangE32();
    }
    if(tc.SPS.sEnable == 1)
        initScanner();

    if(tc.tcpScanEnable == 1)
        iniTcpScanner();

    if(tc.SPS.psEnable == 1)
        initParticleSensor();
}

void MainWindow::iniTcpScanner()
{
    tcpTimer = new QTimer(this);
    socket = new QTcpSocket(this);
    tcpFlag = false;

    connect(socket,SIGNAL(readyRead()),this,SLOT(readTcpScanner()));
    connect(socket,SIGNAL(connected()),this,SLOT(tcpConnected()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(tcpDisconnected()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)));
    connect(tcpTimer,SIGNAL(timeout()),this,SLOT(onProgress()));
    tcpTimer->start(1000);
}

void MainWindow::readTcpScanner()
{
    int i = 0;
    int j = 0;
    QByteArray buffer,buffer1;
    buffer = socket->readAll();

    for(i=1;i<buffer.length();i++)
    {
        if(buffer.at(i) == 13 || buffer.at(i) == 10)
        {
            j = 1;            
            break;
        }else{
            buffer1[i-1] = buffer.at(i);
        }
    }
    QString sn = buffer1;
    if(j == 1)
    {        
        readBarCode(sn);
    }
}

void MainWindow::tcpConnected()
{
    tcpFlag = true;
}

void MainWindow::tcpDisconnected()
{
    tcpFlag = false;
}

void MainWindow::onError(QAbstractSocket::SocketError)
{
    qDebug() << socket->errorString();
}

void MainWindow::onProgress()
{
    if(tcpFlag == false)
    {
        socket->abort();
        socket->connectToHost(tc.tcpScanIP,tc.tcpScanPort,QTcpSocket::ReadWrite);
    }
}

void MainWindow::httpErrorShow()
{
    sDo = 1;
    QMessageBox box(QMessageBox::Warning,tr("数据发送"),tr("数据发送失败，请重新测试!"));
    box.setWindowFlags(Qt::WindowStaysOnTopHint);
    box.setStandardButtons(QMessageBox::Ok);
    box.setButtonText(QMessageBox::Ok,tr("确 定"));
    box.exec();
    box.deleteLater();
    sDo = 0;
}

void MainWindow::sDoSetValue(int v)
{
    sDo = v;
//    ui->lineEdit->setText("");
    if(v == 1)
    {
        ui->lineEdit->setEnabled(false);
    }else{
        ui->lineEdit->setEnabled(true);
        ui->lineEdit->setFocus();
    }
}

void MainWindow::sqlErrorShow(int index)
{
    sDo = 1;

//    ui->lineEdit->setText("");

    if(index == 1)
        info->setText(tr("未能连接数据库!"));
    else
        info->setText(tr("网络差,数据未上传!"));

    info->setStyleSheet("QLabel{color: rgb(255, 0, 0);font: bold;font-size:40px \"宋体\"}");
    info->setGeometry(QRect(500,28,1000,52));
    info->setVisible(true);

    alarmTimer->start(300);
    sDo = 0;
}

void MainWindow::initSql()
{
//    sqlRe = new sqlRemote(this);
//    thread_sqlRe = new QThread(this);

//    sqlRe->moveToThread(thread_sqlRe);

//    connect(thread_sqlRe, SIGNAL(started()), sqlRe, SLOT(startTimer()));
//    connect(thread_sqlRe, SIGNAL(finished()), sqlRe, SLOT(stopTimer()));

//    qRegisterMetaType<QList<testDataStr>>("QList<testDataStr>");
//    connect(this, SIGNAL(writeSql(QStringList,QList<testDataStr>)), sqlRe, SLOT(write(QStringList,QList<testDataStr>)));
//    connect(this, SIGNAL(setIpRemoteFromparameter()), sqlRe, SLOT(setIpRemote()));
//    thread_sqlRe->start();

    thread_sql = new QThread(this);
    sqlOpe.moveToThread(thread_sql);
    qRegisterMetaType<QList<testDataStr>>("QList<testDataStr>");
    connect(this, SIGNAL(writeSql(QStringList,QList<testDataStr>)), &sqlOpe, SLOT(write(QStringList,QList<testDataStr>)));
    connect(&sqlOpe, SIGNAL(httpError()), this, SLOT(httpErrorShow()));
    connect(&sqlOpe, SIGNAL(sqlError(int)), this, SLOT(sqlErrorShow(int)));
    connect(&sqlOpe, SIGNAL(sendsDo(int)), this, SLOT(sDoSetValue(int)));

//    connect(&sqlOpe, SIGNAL(openRemote(QString,int,QString,QString,QString)),sqlRe, SLOT(openDB(QString,int,QString,QString,QString)));
//    connect(sqlRe, SIGNAL(sendRemoteOpen(int)),&sqlOpe, SLOT(getRemoteOpen(int)));

    thread_sql->start();
}

void MainWindow::setItemErrorShow()
{
    info->setText(tr("发送测试项错误，请检查相关设置!"));
    info->setStyleSheet("QLabel{color: rgb(255, 0, 0);font: bold;font-size:40px \"宋体\"}");
    info->setGeometry(QRect(500,28,1000,52));
    info->setVisible(true);

    alarmTimer->start(300);
}

void MainWindow::initSerialWeiHuangE32()
{
    thread_WeiHuangE32 = new QThread(this);
    WeiHuangE32Serial.moveToThread(thread_WeiHuangE32);
    connect(thread_WeiHuangE32, SIGNAL(started()), &WeiHuangE32Serial, SLOT(openSerial()));
    connect(thread_WeiHuangE32, SIGNAL(finished()), &WeiHuangE32Serial, SLOT(closeSerial()));
    connect(this, SIGNAL(SerialPrint(QStringList,QList<testDataStr>)), &WeiHuangE32Serial, SLOT(SerialPrintout(QStringList,QList<testDataStr>)));
    thread_WeiHuangE32->start();
}

void MainWindow::initSerial()
{
    thread_sp = new QThread(this);

    if(tc.machineModel == 1)
    {
        hfSerial.moveToThread(thread_sp);
        connect(thread_sp, SIGNAL(started()), &hfSerial, SLOT(openSerial()));
        connect(thread_sp, SIGNAL(finished()), &hfSerial, SLOT(closeSerial()));

        qRegisterMetaType<testDataStr>("testDataStr");
        connect(&hfSerial, SIGNAL(sendCommand(int,int,testDataStr)), this,
                SLOT(readMessage(int,int,testDataStr)));
        connect(&hfSerial, SIGNAL(sendItemError()), this,SLOT(setItemErrorShow()));

        qRegisterMetaType<groupConfig>("groupConfig");
        connect(this, SIGNAL(sendGroupData(groupConfig)), &hfSerial,
                SLOT(readGroupData(groupConfig)));

        connect(this, SIGNAL(sendCommand(quint16, quint16, QByteArray)), &hfSerial,
                SLOT(serialReadMessage(quint16, quint16, QByteArray)));
    }else if(tc.machineModel == 2){
        mSerial.moveToThread(thread_sp);
        connect(thread_sp, SIGNAL(started()), &mSerial, SLOT(openSerial()));
        connect(thread_sp, SIGNAL(finished()), &mSerial, SLOT(closeSerial()));

        qRegisterMetaType<testDataStr>("testDataStr");
        connect(&mSerial, SIGNAL(sendCommand(int,int,testDataStr)), this,
                SLOT(readMessage(int,int,testDataStr)));

        qRegisterMetaType<groupConfig>("groupConfig");
        connect(this, SIGNAL(sendGroupData(groupConfig)), &mSerial,
                SLOT(readGroupData(groupConfig)));

        connect(this, SIGNAL(sendCommand(quint16, quint16, QByteArray)), &mSerial,
                SLOT(serialReadMessage(quint16, quint16, QByteArray)));
    }else if(tc.machineModel == 3){
        chSerial.moveToThread(thread_sp);
        connect(thread_sp, SIGNAL(started()), &chSerial, SLOT(openSerial()));
        connect(thread_sp, SIGNAL(finished()), &chSerial, SLOT(closeSerial()));

        qRegisterMetaType<testDataStr>("testDataStr");
        connect(&chSerial, SIGNAL(sendCommand(int,int,testDataStr)), this,
                SLOT(readMessage(int,int,testDataStr)));

        qRegisterMetaType<groupConfig>("groupConfig");
        connect(this, SIGNAL(sendGroupData(groupConfig)), &chSerial,
                SLOT(readGroupData(groupConfig)));

        connect(this, SIGNAL(sendCommand(quint16, quint16, QByteArray)), &chSerial,
                SLOT(serialReadMessage(quint16, quint16, QByteArray)));
    }else if(tc.machineModel == 4){
        pplSerial.moveToThread(thread_sp);
        connect(thread_sp, SIGNAL(started()), &pplSerial, SLOT(openSerial()));
        connect(thread_sp, SIGNAL(finished()), &pplSerial, SLOT(closeSerial()));

        qRegisterMetaType<testDataStr>("testDataStr");
        connect(&pplSerial, SIGNAL(sendCommand(int,int,testDataStr)), this,
                SLOT(readMessage(int,int,testDataStr)));

        qRegisterMetaType<groupConfig>("groupConfig");
        connect(this, SIGNAL(sendGroupData(groupConfig)), &pplSerial,
                SLOT(readGroupData(groupConfig)));

        connect(this, SIGNAL(sendCommand(quint16, quint16, QByteArray)), &pplSerial,
                SLOT(serialReadMessage(quint16, quint16, QByteArray)));

        connect(this, SIGNAL(sendAlarm(int)), &pplSerial,SLOT(alarmSlots(int)));
    }else if(tc.machineModel == 5){
        lpcSerial.moveToThread(thread_sp);
        connect(thread_sp, SIGNAL(started()), &lpcSerial, SLOT(openSerial()));
        connect(thread_sp, SIGNAL(finished()), &lpcSerial, SLOT(closeSerial()));

        qRegisterMetaType<testDataStr>("testDataStr");
        connect(&lpcSerial, SIGNAL(sendCommand(int,int,testDataStr)), this,
                SLOT(readMessage(int,int,testDataStr)));

        qRegisterMetaType<groupConfig>("groupConfig");
        connect(this, SIGNAL(sendGroupData(groupConfig)), &lpcSerial,
                SLOT(readGroupData(groupConfig)));

        connect(this, SIGNAL(sendCommand(quint16, quint16, QByteArray)), &lpcSerial,
                SLOT(serialReadMessage(quint16, quint16, QByteArray)));
    }else if(tc.machineModel == 6){
        xsbSerial.moveToThread(thread_sp);
        connect(thread_sp, SIGNAL(started()), &xsbSerial, SLOT(openSerial()));
        connect(thread_sp, SIGNAL(finished()), &xsbSerial, SLOT(closeSerial()));

        qRegisterMetaType<testDataStr>("testDataStr");
        connect(&xsbSerial, SIGNAL(sendCommand(int,int,testDataStr)), this,
                SLOT(readMessage(int,int,testDataStr)));

        qRegisterMetaType<groupConfig>("groupConfig");
        connect(this, SIGNAL(sendGroupData(groupConfig)), &xsbSerial,
                SLOT(readGroupData(groupConfig)));

        connect(this, SIGNAL(sendCommand(quint16, quint16, QByteArray)), &xsbSerial,
                SLOT(serialReadMessage(quint16, quint16, QByteArray)));
    }else{
        serial.moveToThread(thread_sp);
        connect(thread_sp, SIGNAL(started()), &serial, SLOT(openSerial()));
        connect(thread_sp, SIGNAL(finished()), &serial, SLOT(closeSerial()));

        qRegisterMetaType<testDataStr>("testDataStr");
        connect(&serial, SIGNAL(sendCommand(int,int,testDataStr)), this,
                SLOT(readMessage(int,int,testDataStr)));

        qRegisterMetaType<groupConfig>("groupConfig");
        connect(this, SIGNAL(sendGroupData(groupConfig)), &serial,
                SLOT(readGroupData(groupConfig)));

        connect(this, SIGNAL(sendCommand(quint16, quint16, QByteArray)), &serial,
                SLOT(serialReadMessage(quint16, quint16, QByteArray)));
    }
    thread_sp->start();
}

void MainWindow::initParticleSensor()
{
    dustFiltr = new dustFiltration;
    thread_df = new QThread(this);
    dustFiltr->moveToThread(thread_df);
    connect(thread_df, SIGNAL(started()), dustFiltr, SLOT(openSerial()));     //启动
    connect(thread_df, SIGNAL(finished()), dustFiltr, SLOT(closeSerial()));   //关闭
//    connect(this, SIGNAL(sendStop()), &dustFiltr, SLOT(readStop()));
//    connect(this, SIGNAL(sendScanner(int)), &dustFiltr, SLOT(readScanner(int)));
    connect(dustFiltr, SIGNAL(sendSensor1Data(float)), this, SLOT(readSensor1(float)));
    connect(dustFiltr, SIGNAL(sendSensor2Data(float)), this, SLOT(readSensor2(float)));
//    connect(dustFiltr, SIGNAL(sendSensor3Data(float)), this, SLOT(readSensor3(float)));
    thread_df->start();
}

void MainWindow::initScanner()
{
//    thread_sca = new QThread(this);
//    scannerOpe.moveToThread(thread_sca);
//    connect(thread_sca, SIGNAL(started()), &scannerOpe, SLOT(openScanner()));     //启动
//    connect(thread_sca, SIGNAL(finished()), &scannerOpe, SLOT(closeScanner()));   //关闭
//    connect(this, SIGNAL(sendStop()), &scannerOpe, SLOT(readStop()));
//    connect(this, SIGNAL(sendScanner(int)), &scannerOpe, SLOT(readScanner(int)));
//    connect(&scannerOpe, SIGNAL(sendBarCode(QString)), this, SLOT(readBarCode(QString)));
//    thread_sca->start();
    openScanner();
}

void MainWindow::closeScanner()
{
    if(scannerPort->isOpen())
        scannerPort->close();
    disconnect(scannerPort, SIGNAL(readyRead()), this, SLOT(readScannerData()));
    scannerPort = NULL;
    tc.sPortOpen = 0;
}

void MainWindow::openScanner()
{
    if(scannerPort  ==  NULL)
        scannerPort = new QSerialPort(this);

    if(scannerPort->isOpen())
        scannerPort->close();

    QString portname;
    if(tc.SPS.sPort >= 10)
    {
        portname.append("\\\\.\\").append("com"+QString::number(tc.SPS.sPort));
    }else{
        portname.append("com"+QString::number(tc.SPS.sPort));
    }

    scannerPort->setPortName(portname);

//    scannerPort->setPortName("com"+QString::number(tc.SPS.sPort));
    scannerPort->setBaudRate(tc.SPS.sBaudRate);     //波特率
    scannerPort->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    scannerPort->setDataBits(QSerialPort::Data8);        //数据位设置，设置为8位数据位
    scannerPort->setStopBits(QSerialPort::OneStop);      //停止位设置，设置为1位停止位
    scannerPort->setFlowControl(QSerialPort::NoFlowControl); //控制流
    scannerPort->setReadBufferSize(1024);                //缓冲区大小
    bool ok = scannerPort->open(QSerialPort::ReadWrite);

    if(ok)
    {
        tc.sPortOpen = 1;
        connect(scannerPort, SIGNAL(readyRead()), this, SLOT(readScannerData()));
    }else{
        tc.sPortOpen = 0;
    }
}

void MainWindow::readScannerData()
{
    int i = 0;
    int j = 0;
    barCodeScan.append(scannerPort->readAll());
    if (barCodeScan.contains("\r")) {
        int r = barCodeScan.indexOf("\r");
        QString msg = barCodeScan.mid(0, r);
        barCodeScan.clear();
        readBarCode(msg);
    }
}

/************************************
 * 方法名：testDataItemSize
 * 说明：
************************************/
void MainWindow::testDataItemSize(int step)
{
    int h = ui->listView->size().height()/4;
    int w = ui->listView->size().width();
    int num = testItem_List.count();

    for(int i=0;i<num;i++)
    {
        if(step == i)
        {
            standardItem_List[i]->setSizeHint(QSize(0,h));
            testItem_List[i]->setItemSize(w,h);
            testItem_List[i]->setItemPix(1);
        }else{
            standardItem_List[i]->setSizeHint(QSize(0,h/2));
            testItem_List[i]->setItemSize(w,h/2);

            if(i > step)
                testItem_List[i]->setItemPix(0);
        }
    }
}

/************************************
 * 方法名：readMessage
 * 说明：
************************************/
void MainWindow::readMessage(int index, int cmd, testDataStr tds)
{
//    if(index == -1)
//    {
//        if(remind == 0)
//        {
//            remind = 1;
//            QMessageBox box(QMessageBox::Warning,tr("通讯异常"),tr("与下位机通讯异常，请检查串口设置是否正确!"));
//            box.setWindowFlags(Qt::WindowStaysOnTopHint);
//            box.setStandardButtons(QMessageBox::Ok);
//            box.setButtonText(QMessageBox::Ok,tr("确 定"));
//            box.exec();
//            box.deleteLater();
//            remind = 0;
//        }
//        return;
//    }

    if(index == -1)
    {
        if(remind == 0)
        {
            remind = 1;

            info->setText(tr("与下位机通讯异常，请检查串口设置是否正确！"));
            info->setStyleSheet("QLabel{color: rgb(255, 0, 0);font: bold;font-size:40px \"宋体\"}");
            info->setGeometry(QRect(500,28,1100,52));
            info->setVisible(true);
            alarmTimer->start(300);
        }/*else{
            if(ui->textBrowser->styleSheet() == "QTextBrowser{color: rgb(255, 0, 0);}")
                ui->textBrowser->setStyleSheet("QTextBrowser{color: rgb(255, 165, 8);}");
            else
                ui->textBrowser->setStyleSheet("QTextBrowser{color: rgb(255, 0, 0);}");
        }*/
        return;
    }else{
        if(index == 1 && iniFlag == 1)
        {
            alarmTimer->stop();
            info->setVisible(false);

            sendGroupData(groupData);
            iniFlag = -1;
        }else if(remind == 1){
            alarmTimer->stop();
            info->setVisible(false);

            if(index == 1)
                sendGroupData(groupData);
            remind = 0;
        }
    }    

    int c = cmd;
    int s = tc.state;
    float ft = 0.0;
    tc.state = index;
    switch(index)
    {        
        case 0:
            saveDb = 0;
            testStart = 0;
            testing = 0;
            PASS = 1;
            iniFlag = 1;
            currentMax = 0.0;
            currentMax1 = 0.0;
            ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
            ui->label_17->setText(tr("初始"));
            break;
        case 1:
            saveDb = 0;
            testing = 0;
            testStart = 0;
            PASS = 1;
            currentMax = 0.0;
            currentMax1 = 0.0;
            if(groupData.groupNum == 0)
                selfOffset = 1;
            else
                selfOffset = 0;

            ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
                ui->label_17->setText(tr("待机"));
            ui->lineEdit->setFocus();
            break;
        case 2:
            saveDb = 0;
            testing = 0;
            testStart = 0;
            PASS = 1;
            currentMax = 0.0;
            currentMax1 = 0.0;
            ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
            ui->label_17->setText(tr("设置"));
            break;
        case 3:
        case 4:
            {
                if(index == 3)
                {
                    if(testing == 0)
                    {
                        if(info->isVisible() && info->text() != tr("发送测试项错误，请检查相关设置!"))
                        {
                            alarmTimer->stop();
                            info->setText("");
                            info->setVisible(false);
                        }

                        if(tc.codeMode == 0)
                        {
                            QString sn = QString("%1%2%3").arg(tc.AC.prefix)
                                    .arg(tc.AC.lastCode,tc.AC.length,10,QChar('0'))
                                    .arg(tc.AC.suffix);
                            tc.barCode = sn;
                            ui->lineEdit->setText(sn);
                        }
                        tc.barCode = ui->lineEdit->text();
                        ui->lineEdit->setEnabled(false);                        

                        testing = 1;
                        PASS = 1;
                        currentMax = 0.0;
                        currentMax1 = 0.0;

                        if(tds.group != 0)
                            clearGroupTest(1);
                        else
                            clearGroupTest(0);
                    }

                    saveDb = 0;
                    ui->label_17->setStyleSheet("QLabel{background-color: rgb(255, 255, 0);}");
                    ui->label_17->setText(tr("测试中"));
                }

                if(tds.model > 8 || tds.model <= 0)
                {
                    break;
                }

                if(testStart == 0 || testStart != tds.step)
                {
                    if(testStart != tds.step)
                    {
                        currentMax = 0.0;
                        currentMax1 = 0.0;
                        xlFlag = -1;
                        testItem_List[testStart]->showLLTData2(rangeSets[4].prec[1]);
                    }
                    testStart = tds.step;
                    testDataItemSize(tds.step);
                    qindex = model->index(tds.step,0);
                    ui->listView->setCurrentIndex(qindex);
                }

                if(tds.group == 0)
                    selfOffset = 1;

                if(tds.testTime >= 3276.80)
                    ft = tds.testTime - 3276.80;
                else
                    ft = tds.testTime;

                if(tds.model == 2)
                    psCanDo = 1;
                else
                    psCanDo = -1;

                QStringList dataShow;
                switch(tds.model)
                {
                    case 1:
                        if(selfOffset != 1 && groupData.downTime > 0.0 && tds.testTime == 0.0)
                            tds.voltage = 0.0;
                        else if(selfOffset == 1 && groupOffset.downTime > 0.0 && tds.testTime == 0.0)
                            tds.voltage = 0.0;

                        dataShow<<QString::number(tds.voltage,10,rangeSets[0].prec[0]);
                        dataShow<<QString::number(tds.current,10,rangeSets[0].prec[1]);
                        dataShow<<"";
                        dataShow<<QString::number(ft,10,rangeSets[0].prec[3]);
                        break;
                    case 2:
                        if(selfOffset != 1 && groupData.downTime > 0.0 && tds.testTime == 0.0)
                            tds.voltage = 0.0;
                        else if(selfOffset == 1 && groupOffset.downTime > 0.0 && tds.testTime == 0.0)
                            tds.voltage = 0.0;

                        dataShow<<QString::number(tds.voltage,10,rangeSets[1].prec[0]);
                        dataShow<<QString::number(tds.current,10,rangeSets[1].prec[1]);
                        dataShow<<"";
                        dataShow<<QString::number(ft,10,rangeSets[1].prec[3]);
                        break;
                    case 3:
                        dataShow<<QString::number(tds.voltage,10,rangeSets[2].prec[0]);

                        if(tds.resistance > rangeSets[2].max[1])
//                            dataShow<<QString(">%1%2").arg(rangeSets[2].max[1]).arg(rangeSets[2].unit[1]);
                            dataShow<<QString(">%1").arg(rangeSets[2].max[1]);
                        else
                            dataShow<<QString::number(tds.resistance,10,rangeSets[2].prec[1]);
                        dataShow<<"";
                        dataShow<<QString::number(ft,10,rangeSets[2].prec[3]);
                        break;
                    case 4:                        
                        dataShow<<QString::number(tds.current,10,rangeSets[3].prec[3]);
                        if(groupData.gnd == 0)
                        {
                            if(tds.resistance > rangeSets[3].max[1])
//                                dataShow<<QString(">%1%2").arg(rangeSets[3].max[1]).arg(rangeSets[3].unit[1]);
                                dataShow<<QString(">%1").arg(rangeSets[3].max[1]);
                            else
                                dataShow<<QString::number(tds.resistance,10,rangeSets[3].prec[1]);
                        }else{
                            if(tds.voltage > rangeSets[3].max[0])
//                                dataShow<<QString(">%1%2").arg(rangeSets[3].max[0]).arg(rangeSets[3].unit[0]);
                                dataShow<<QString(">%1").arg(rangeSets[3].max[0]);
                            else
                                dataShow<<QString::number(tds.voltage,10,rangeSets[3].prec[1]);
                        }
                        dataShow<<"";
                        dataShow<<QString::number(ft,10,rangeSets[3].prec[2]);
                        break;
                    case 5:
                        dataShow<<QString::number(tds.voltage,10,rangeSets[4].prec[0]);
                        dataShow<<QString::number(tds.current,10,rangeSets[4].prec[1]);
                        if(tc.phase == 1 || tc.phase == 2)
                            dataShow<<QString::number(tds.pCurrent,10,0);
                        else
                            dataShow<<"";
                        dataShow<<QString::number(ft,10,rangeSets[4].prec[3]);
//                        qDebug()<<ft<<tds.current;
                        if(groupData.items[tds.step].data[3] > 1.0)
                        {
                            if(ft > groupData.items[tds.step].data[3]/2.0)
                            {
                                currentMax = tds.current;
                                xlFlag = 1;
                            }else{
                                currentMax1 = tds.current;
                                xlFlag = 0;
                            }
                            tds.current = currentMax>currentMax1?currentMax:currentMax1;
                        }
                        break;
                    case 6:
                        dataShow<<QString::number(tds.voltage,10,rangeSets[5].prec[0]);
                        dataShow<<QString::number(tds.current,10,rangeSets[5].prec[3]);
                        dataShow<<QString::number(tds.power,10,rangeSets[5].prec[1]);
                        dataShow<<QString::number(ft,10,rangeSets[5].prec[2]);
                        break;
                    case 7:
                        dataShow<<QString::number(tds.voltage,10,rangeSets[6].prec[0]);
                        dataShow<<QString::number(tds.current,10,rangeSets[6].prec[1]);
                        dataShow<<QString::number(tds.power,10,rangeSets[6].prec[3]);
                        dataShow<<QString::number(ft,10,rangeSets[6].prec[2]);
                        break;
                    case 8:
                        dataShow<<QString::number(tds.voltage,10,rangeSets[7].prec[0]);
                        dataShow<<QString::number(tds.resistance,10,rangeSets[7].prec[1]);
                        dataShow<<"";
                        dataShow<<QString::number(ft,10,rangeSets[7].prec[3]);
                        break;
                }

                if(dataShow.size() > 0 && saveDb == 0)
                    testItem_List[tds.step]->setTestData(xlFlag,dataShow);
//                dataShow.clear();

                if(tds.testTime == 0.0)
                {
                    if(tds.flag != 0 && saveDb == 0)
                    {
                        testItem_List[tds.step]->testDataList<<tds;
                    }

                    if(tds.flag1 == 1)
                    {
                        testItem_List[tds.step]->setItemPix(2);
                    }else{
                        testItem_List[tds.step]->setItemPix(3);
                        PASS = 0;
                    }
                }

                if(index == 4 && cmd == 1)
                {
                    QStringList list;
                    if (tc.PS.PrintType == 1  && tc.PS.autoPrint == 1)
                    {
                        if(((tc.PS.printFail && PASS == 0) || (tc.PS.printPass && PASS == 1)) && saveDb == 0)
                        {
                            qDebug()<<"print1";
                            if(tds.group != 0)
                            {
                                list.clear();
                                testDataList.clear();
                                testItem_List[tds.step]->showLLTData2(rangeSets[4].prec[1]);
                                for(int i=0;i<testItem_List.count();i++)
                                {
                                    if(testItem_List[i]->testDataList.isEmpty())
                                        continue;
                                    testDataList<<testItem_List[i]->testDataList.last();
                                }
                                list<<tc.barCode<<tc.currentGroup<<tc.userName<<QString::number(PASS,10);
                                list<<groupData.condition;
                                SerialPrint(list,testDataList);
                            }
                        }
                    }
                    if(((tc.STD.savaFail && PASS == 0) || (tc.STD.savePass && PASS == 1)) && saveDb == 0)
                    {
                        if(tds.group != 0)
                        {
                            list.clear();
                            testDataList.clear();
                            testItem_List[tds.step]->showLLTData2(rangeSets[4].prec[1]);
                            for(int i=0;i<testItem_List.count();i++)
                            {
                                if(testItem_List[i]->testDataList.isEmpty())
                                    continue;

                                testDataList<<testItem_List[i]->testDataList.takeLast();
                                testItem_List[i]->testDataList.clear();
                            }

                            for(int i=0;i<testDataList.size();i++)
                            {
                                switch(testDataList.at(i).model)
                                {
                                case 1:
                                    tc.iNum[0]++;
                                    break;
                                case 2:
                                    tc.iNum[1]++;
                                    break;
                                case 3:
                                    tc.iNum[2]++;
                                    break;
                                case 4:
                                    tc.iNum[3]++;
                                    break;
                                case 5:
                                    tc.iNum[4]++;
                                    break;
                                case 6:
                                    tc.iNum[5]++;
                                    break;
                             case 7:
                                    tc.iNum[6]++;
                                    break;
                                case 8:
                                    tc.iNum[7]++;
                                    break;
                                }

                                if(testDataList.at(i).flag1 == 0)
                                    tc.iNGNum[testDataList.at(i).model - 1]++;
                            }
                            list<<tc.barCode<<tc.currentGroup<<tc.userName<<QString::number(PASS,10);                            
                            list<<groupData.condition;
                            mdbFileFun();
                            qDebug()<<"write1"<<list;
                            writeSql(list,testDataList);                           
                            currentMax = 0.0;
                            currentMax1 = 0.0;
                            testing = 0;
                            testStart = 0;
                            tc.barCode = "";
                            ui->lineEdit->setText("");
                            list.clear();
                            testDataList.clear();

                            tc.total++;
                            tc.iTotal++;
                            if(PASS == 1)
                            {
                                tc.passNum++;
                                tc.iPass++;
                                if(tc.machineModel == 4)
                                    emit sendAlarm(2);
                            }else{
                                if(tc.machineModel == 4)
                                    emit sendAlarm(4);
                            }

                            if(tc.total > 0 && tc.passNum <= tc.total)
                            {
                                QString r = QString::number(((float)tc.passNum/(float)tc.total)*100.0,10,2)+"%";
                                ui->label_25->setText(r);

                                r = QString::number(tc.total,10,0);
                                ui->label_21->setText(r);
                            }

                            if(tc.codeMode == 0)
                            {
                                if(tc.AC.lastCode <= 0 || tc.AC.lastCode >= 999999999)
                                    tc.AC.lastCode = 0;

                                if(PASS == 1)
                                {
                                    if(tc.AC.down == 0)
                                        tc.AC.lastCode++;
                                    else
                                        tc.AC.lastCode--;
                                }else{
                                    if(tc.AC.keep == 0)
                                    {
                                        if(tc.AC.down == 0)
                                            tc.AC.lastCode++;
                                        else
                                            tc.AC.lastCode--;
                                    }
                                }
                            }

                            refreshBarChart();
                        }
                        saveDb++;

                        if(tds.group == 0)
                            selfOffset = 1;

                        if(selfOffset == 1)
                        {
                            if(PASS == 0)
                            {
                                ui->label_17->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
                                ui->label_17->setText(tr("不合格"));
                            }else{
                                ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
                                ui->label_17->setText(tr("合格"));
                            }

                            QMessageBox box(QMessageBox::Information,tr("线路补偿"),tr("自检完成!"));
                            box.setWindowFlags(Qt::WindowStaysOnTopHint);
                            box.setStandardButtons(QMessageBox::Ok);
                            box.setButtonText(QMessageBox::Ok,tr("确 定"));
                            box.exec();

                            selfOffset = 0;
                            readGroupFromFile(tc.currentGroup);
                            sendGroupData(groupData);
                            delay(1000);
                            showGroup();
                        }
                        ui->lineEdit->setText("");
                        tc.barCode = "";
                    }

                    if(PASS == 0)
                    {                       
                        ui->label_17->setStyleSheet("QLabel{background-color: rgb(255, 0, 0);}");
                        ui->label_17->setText(tr("不合格"));
                    }else{
                        ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
                        ui->label_17->setText(tr("合格"));
                    }
//                    qDebug()<<"sDo"<<sDo;
                    ui->lineEdit->setEnabled(true);
                    ui->lineEdit->setFocus();                    

                    if((tc.STD.savaFail == 0 || tc.STD.savePass == 0) && saveDb == 0)
                    {
                        ui->lineEdit->setText("");
                        tc.barCode = "";
                        sDo = 0;
                        saveDb++;
                    }
                }
            }
            break;
        case 5:
            saveDb = 0;
            testStart = 0;
            PASS = 1;
            currentMax = 0.0;
            currentMax1 = 0.0;
            ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
            ui->label_17->setText(tr("系统"));
            break;
        case 6:
            saveDb = 0;
            testStart = 0;
            PASS = 1;
            currentMax = 0.0;
            currentMax1 = 0.0;
            ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
            ui->label_17->setText(tr("调试"));
            break;
        case 7:
            saveDb = 0;
            testStart = 0;
            PASS = 1;
            currentMax = 0.0;
            currentMax1 = 0.0;
            ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
            ui->label_17->setText(tr("错误"));
            break;
        default:
            saveDb = 0;
            testStart = 0;
            PASS = 1;
            currentMax = 0.0;
            currentMax1 = 0.0;
            ui->label_17->setStyleSheet("QLabel{background-color: rgb(0, 255, 0);}");
//            ui->label_17->setText("");
            qDebug()<<"error"<<index;
    }
}

void MainWindow::iniBarChart()
{
    set0 = new QBarSet("");
    set1 = new QBarSet("");

    set0->setColor(Qt::red);
    set1->setColor(Qt::blue);

    set0->append(tc.iNGNum[0]);
    set0->append(tc.iNGNum[1]);
    set0->append(tc.iNGNum[2]);
    set0->append(tc.iNGNum[3]);
    set0->append(tc.iNGNum[4]);
    set0->append(tc.iNGNum[5]);
    set0->append(tc.iNGNum[6]);
    set0->append(tc.iNGNum[7]);

    set1->append(tc.iNum[0]);
    set1->append(tc.iNum[1]);
    set1->append(tc.iNum[2]);
    set1->append(tc.iNum[3]);
    set1->append(tc.iNum[4]);
    set1->append(tc.iNum[5]);
    set1->append(tc.iNum[6]);
    set1->append(tc.iNum[7]);

    series = new QBarSeries();

    series->append(set0);
    series->append(set1);

    chart = new QChart();
    axis = new QBarCategoryAxis();

    QFont font("Microsoft YaHei",7,QFont::Normal);
    QStringList categories;
    categories << tr("交耐") << tr("直耐") << tr("绝缘") << tr("接地") << tr("泄漏") << tr("功率") << tr("低启") << tr("开短");

    axis->append(categories);
    axis->setLabelsFont(font);

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);
}

void MainWindow::barChartMemoryClear()
{
    if(set0 != NULL)
        {
            delete set0;
            set0 = NULL;
        }

    if(set1 != NULL)
        {
            delete set1;
            set1 = NULL;
        }

    if(series != NULL)
        {
    //        series->deleteLater();
            delete series;
            series = NULL;
        }

    if(chart = NULL)
        {
    //        chart->deleteLater();
            delete chart;
            chart = NULL;
        }

     if(axis != NULL)
        {
    //        axis->deleteLater();
            delete axis;
            axis = NULL;
        }
}

/*****************************
 * 方法名：refreshBarChart
 * 说明：
*****************************/
void MainWindow::refreshBarChart()
{   

    QFont font("Microsoft YaHei",7,QFont::Normal);
    if(set0 != NULL)
    {
        delete set0;
        set0 = NULL;
    }
    set0 = new QBarSet("");

    if(set1 != NULL)
    {
        delete set1;
        set1 = NULL;
    }
    set1 = new QBarSet("");

    set0->append(tc.iNGNum[0]);
    set0->append(tc.iNGNum[1]);
    set0->append(tc.iNGNum[2]);
    set0->append(tc.iNGNum[3]);
    set0->append(tc.iNGNum[4]);
    set0->append(tc.iNGNum[5]);
    set0->append(tc.iNGNum[6]);
    set0->append(tc.iNGNum[7]);

    set1->append(tc.iNum[0]);
    set1->append(tc.iNum[1]);
    set1->append(tc.iNum[2]);
    set1->append(tc.iNum[3]);
    set1->append(tc.iNum[4]);
    set1->append(tc.iNum[5]);
    set1->append(tc.iNum[6]);
    set1->append(tc.iNum[7]);

    set0->setColor(Qt::red);
    set1->setColor(Qt::blue);

    if(series != NULL)
    {
        delete series;
        series = NULL;
    }
    series = new QBarSeries();
    series->append(set0);
    series->append(set1);
    if(chart != NULL)
    {
        chart->deleteLater();
        chart = NULL;
    }

    QString r;
    if(tc.iTotal <= 0 || tc.iTotal < tc.iPass)
        r = "0.0%";
    else
        r = QString::number(((float)tc.iPass/(float)tc.iTotal)*100.0,10,2)+"%";

    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(QString("%1：%2    %3：%4    %5：%6").arg(tr("今日总量")).arg(tc.iTotal)
                    .arg(tr("合格数")).arg(tc.iPass).arg(tr("合格率")).arg(r));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << tr("交耐") << tr("直耐") << tr("绝缘") << tr("接地") << tr("泄漏") << tr("功率") << tr("低启") << tr("开短");

    if(axis != NULL)
    {
//        axis->deleteLater();
        delete axis;
        axis = NULL;
    }
    axis = new QBarCategoryAxis();
    axis->append(categories);
    axis->setLabelsFont(font);

    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView->setChart(chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

/*****************************
 * 方法名：getBaudRate
 * 说明：返回对应的波特率
*****************************/
static QSerialPort::BaudRate getBaudRate(int baudRate)
{
    switch(baudRate)
    {
        case 1200:
            return QSerialPort::Baud1200;
        case 2400:
            return QSerialPort::Baud2400;
        case 4800:
            return QSerialPort::Baud4800;
        case 9600:
            return QSerialPort::Baud9600;
        case 19200:
            return QSerialPort::Baud19200;
        case 38400:
            return QSerialPort::Baud38400;
        case 57600:
            return QSerialPort::Baud57600;
        case 115200:
            return QSerialPort::Baud115200;
        default:
            return QSerialPort::UnknownBaud;
    }
}

/****************************
 * 方法名：openPort
 * 说明：打开串口
****************************/
bool MainWindow::openPort(QSerialPort *sp,int port,int baudRate)
{
    if (sp->isOpen())
    {
        return true;
    }
    sp->setPortName("com"+QString::number(port));
    sp->setBaudRate(getBaudRate(baudRate));     //波特率
    sp->setParity(QSerialPort::NoParity);       //奇偶校验设置，当前为无校验
    sp->setDataBits(QSerialPort::Data8);        //数据位设置，我们设置为8位数据位
    sp->setStopBits(QSerialPort::OneStop);      //停止位设置，我们设置为1位停止位
    sp->setFlowControl(QSerialPort::NoFlowControl); //控制流
    sp->setReadBufferSize(1024);                //缓冲区大小
    return sp->open(QSerialPort::ReadWrite);
}

/******************************
 * 方法名：writeIniFile
 * 说明：
******************************/
void MainWindow::writeIniFile()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QSettings *configIniWrite = new QSettings(QCoreApplication::applicationDirPath()+"/setInfo.ini",
                                              QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("GBK"));

    configIniWrite->setValue("/main/userName",tc.userName);
    configIniWrite->setValue("main/language",QString("%1").arg(tc.language));
    configIniWrite->setValue("main/startMode",QString("%1").arg(tc.startMode));
    configIniWrite->setValue("main/machineModel",QString("%1").arg(tc.machineModel));
    configIniWrite->setValue("/main/loginDate",current_date_time.toString("yyyy-MM-dd hh:mm:ss"));
    configIniWrite->setValue("set/addr",QString("%1").arg(tc.SPS.machineAddr));
    configIniWrite->setValue("set/port",QString("%1").arg(tc.SPS.port));
    configIniWrite->setValue("set/baudRate",QString("%1").arg(tc.SPS.baudRate));
    configIniWrite->setValue("set/sPort",QString("%1").arg(tc.SPS.sPort));
    configIniWrite->setValue("set/sBaudRate",QString("%1").arg(tc.SPS.sBaudRate));
    configIniWrite->setValue("set/sEnable",QString("%1").arg(tc.SPS.sEnable));
    configIniWrite->setValue("set/pplPort",QString("%1").arg(tc.SPS.pplPort));
    configIniWrite->setValue("set/pplBaudRate",QString("%1").arg(tc.SPS.pplBaudRate));

    configIniWrite->setValue("set/psPort1",QString("%1").arg(tc.SPS.psPort1));
    configIniWrite->setValue("set/psBaudRate1",QString("%1").arg(tc.SPS.psBaudRate1));
    configIniWrite->setValue("set/psPort2",QString("%1").arg(tc.SPS.psPort2));
    configIniWrite->setValue("set/psBaudRate2",QString("%1").arg(tc.SPS.psBaudRate2));
    configIniWrite->setValue("set/psEnable",QString("%1").arg(tc.SPS.psEnable));
    configIniWrite->setValue("set/psCoefficient1",QString("%1").arg(tc.SPS.psCoefficient1));
    configIniWrite->setValue("set/psCoefficient2",QString("%1").arg(tc.SPS.psCoefficient2));
    configIniWrite->setValue("set/psCoefficient3",QString("%1").arg(tc.SPS.psCoefficient3));
    configIniWrite->setValue("set/psCoefficient4",QString("%1").arg(tc.SPS.psCoefficient4));
    configIniWrite->setValue("set/psMin",QString("%1").arg(tc.SPS.psMin));
    configIniWrite->setValue("set/psMax",QString("%1").arg(tc.SPS.psMax));

    configIniWrite->setValue("set/tcpScanEnable",QString("%1").arg(tc.tcpScanEnable));
    configIniWrite->setValue("set/tcpScanPort",QString("%1").arg(tc.tcpScanPort));
    configIniWrite->setValue("set/tcpScanIP",QString("%1").arg(tc.tcpScanIP));
    configIniWrite->setValue("set/cPort",QString("%1").arg(tc.SPS.cPort));
    configIniWrite->setValue("set/cBaudRate",QString("%1").arg(tc.SPS.cBaudRate));
    configIniWrite->setValue("set/lastGroup",tc.currentGroup);
    configIniWrite->setValue("set/testNum",QString("%1").arg(tc.total));
    configIniWrite->setValue("set/phase",QString("%1").arg(tc.phase));
    configIniWrite->setValue("set/md",QString("%1").arg(tc.md));
    configIniWrite->setValue("set/itemNumMax",QString("%1").arg(tc.itemNumMax));

    configIniWrite->setValue("set/passNum",QString("%1").arg(tc.passNum));
    configIniWrite->setValue("set/codeMode",QString("%1").arg(tc.codeMode));
    configIniWrite->setValue("set/auxiliary1",tc.auxiliary1);
    configIniWrite->setValue("set/auxiliary2",tc.auxiliary2);
    configIniWrite->setValue("set/auxiliary3",tc.auxiliary3);
    configIniWrite->setValue("set/txtEnable",QString("%1").arg(tc.txtEnable));
    configIniWrite->setValue("set/txtFile",tc.txtFile);
    configIniWrite->setValue("set/url1",tc.url_1);
    configIniWrite->setValue("set/url2",tc.url_2);
    configIniWrite->setValue("set/url3",tc.url_3);
    configIniWrite->setValue("set/leakageOffet",QString("%1").arg(tc.SP.leakageOffet));
    configIniWrite->setValue("set/gndOffset",QString("%1").arg(tc.SP.GNDOffset));
    configIniWrite->setValue("set/acOffset",QString("%1").arg(tc.SP.ACOffset));
    configIniWrite->setValue("set/dcOffset",QString("%1").arg(tc.SP.DCOffset));
    configIniWrite->setValue("set/ground",QString("%1").arg(tc.SP.ground));
    configIniWrite->setValue("set/alarm",QString("%1").arg(tc.SP.alarm));
    configIniWrite->setValue("set/plc",QString("%1").arg(tc.SP.plc));
    configIniWrite->setValue("set/prefix",tc.AC.prefix);
    configIniWrite->setValue("set/suffix",tc.AC.suffix);
    configIniWrite->setValue("set/code",tc.AC.code);
    configIniWrite->setValue("set/lastCode",tc.AC.lastCode);
    configIniWrite->setValue("set/length",QString("%1").arg(tc.AC.length));
    configIniWrite->setValue("set/autoDown",QString("%1").arg(tc.AC.down));
    configIniWrite->setValue("set/autoKeep",QString("%1").arg(tc.AC.keep));

    configIniWrite->setValue("set/setByBarCode",QString("%1").arg(tc.BI.setByBarCode));
    configIniWrite->setValue("set/startByBarcode",QString("%1").arg(tc.BI.startByBarCode));
    configIniWrite->setValue("set/lengthCheck",QString("%1").arg(tc.BI.lengthCheck));
    configIniWrite->setValue("set/barCodeLength",QString("%1").arg(tc.BI.barCodeLength));

    configIniWrite->setValue("set/saveAll",QString("%1").arg(tc.STD.saveAll));
    configIniWrite->setValue("set/savePass",QString("%1").arg(tc.STD.savePass));
    configIniWrite->setValue("set/saveFail",QString("%1").arg(tc.STD.savaFail));
    configIniWrite->setValue("set/doNotSave",QString("%1").arg(tc.STD.doNotSave));
    configIniWrite->setValue("set/upload",QString("%1").arg(tc.STD.upload));
    configIniWrite->setValue("set/uploadType",QString("%1").arg(tc.STD.uploadType));
    configIniWrite->setValue("set/repeat",QString("%1").arg(tc.STD.repeat));
    configIniWrite->setValue("set/repeatType",QString("%1").arg(tc.STD.repeatType));

    configIniWrite->setValue("set/dbIp",tc.DS.ip);
    configIniWrite->setValue("set/dbName",tc.DS.dbName);
    configIniWrite->setValue("set/loginName",tc.DS.loginName);
    configIniWrite->setValue("set/password",tc.DS.password);
    configIniWrite->setValue("set/machineNum",tc.DS.machineNum);
    configIniWrite->setValue("set/dbPort",QString("%1").arg(tc.DS.port));

    configIniWrite->setValue("set/iDate",current_date_time.toString("yyyy-MM-dd"));
    configIniWrite->setValue("set/iTotal",QString("%1").arg(tc.iTotal));
    configIniWrite->setValue("set/iPass",QString("%1").arg(tc.iPass));
    configIniWrite->setValue("set/iNum",QString("%1,%2,%3,%4,%5,%6,%7,%8").arg(tc.iNum[0]).arg(tc.iNum[1]).arg(tc.iNum[2]).arg(tc.iNum[3]).arg(tc.iNum[4]).arg(tc.iNum[5]).arg(tc.iNum[6]).arg(tc.iNum[7]));
    configIniWrite->setValue("set/iNGNum",QString("%1,%2,%3,%4,%5,%6,%7,%8").arg(tc.iNGNum[0]).arg(tc.iNGNum[1]).arg(tc.iNGNum[2]).arg(tc.iNGNum[3]).arg(tc.iNGNum[4]).arg(tc.iNGNum[5]).arg(tc.iNGNum[6]).arg(tc.iNGNum[7]));

    configIniWrite->setValue("set/autoPrint",QString("%1").arg(tc.PS.autoPrint));
    configIniWrite->setValue("set/PrintType",QString("%1").arg(tc.PS.PrintType));
    configIniWrite->setValue("set/printFail",QString("%1").arg(tc.PS.printFail));
    configIniWrite->setValue("set/printPass",QString("%1").arg(tc.PS.printPass));
    configIniWrite->setValue("set/PrintPort",QString("%1").arg(tc.PS.PrintPort));
    configIniWrite->setValue("set/PrintBaudRate",QString("%1").arg(tc.PS.PrintBaudRate));

    configIniWrite->setValue("set/ACW_I_K",QString("%1").arg(tc.ACW_I_K));

    //写入完成后删除指针
    delete configIniWrite;
}

/*************************************
 * 方法名：clearGroupTest
 * 说明：
*************************************/
void MainWindow::clearGroupTest(int flag)
{
    QStringList unit,data;

    int num = testItem_List.count();

    if(flag == 1)
    {
        for(int i=0;i<num;i++)
        {
            data.clear();
            unit.clear();
            if(groupData.items[i].itemName == tr("无效"))
            {
                data<<""<<""<<""<<"";
                testItem_List[i]->setItemName("------");
            }else{
                switch(groupData.items[i].id)
                {
                case 1:
                    data<<QString::number(0.0,10,rangeSets[0].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[0].prec[1]);
                    data<<"";
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[0].prec[3]);
                    break;
                case 2:
                    data<<QString::number(0.0,10,rangeSets[1].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[1].prec[1]);
                    data<<"";
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[1].prec[3]);
                    break;
                case 3:
                    data<<QString::number(0.0,10,rangeSets[2].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[2].prec[1]);
                    data<<"";
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[2].prec[3]);
                    break;
                case 4:
                    data<<QString::number(0.0,10,rangeSets[3].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[3].prec[1]);
                    data<<"";
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[3].prec[2]);
                    break;
                case 5:
                    data<<QString::number(0.0,10,rangeSets[4].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[4].prec[1]);
                    data<<"";
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[4].prec[3]);
                    break;
                case 6:
                    data<<QString::number(0.0,10,rangeSets[5].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[5].prec[3]);
                    data<<QString::number(0.0,10,rangeSets[5].prec[1]);
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[5].prec[2]);
                    break;
                case 7:
                    data<<QString::number(0.0,10,rangeSets[6].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[6].prec[1]);
                    data<<QString::number(0.0,10,rangeSets[6].prec[3]);
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[6].prec[2]);
                    break;
                case 8:
                    data<<QString::number(0.0,10,rangeSets[7].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[7].prec[1]);
                    data<<"";
                    data<<QString::number(groupData.items[i].data[3],10,rangeSets[7].prec[3]);
                    break;
                }
                testItem_List[i]->setItemName(groupData.items[i].itemName);
            }

            if(groupData.items[i].id > 0)
            {
                if(groupData.items[i].id == 4)
                {
                    if(groupData.gnd == 0)
                    {
                        unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电阻")+QString("(%1)").arg(rangeSets[3].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                    }else{
                        unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电压")+QString("(%1)").arg(rangeSets[3].unit[0])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                    }
                    testItem_List[i]->setItemUnit(unit);
                }else{
                    testItem_List[i]->setItemUnit(itemUnits[groupData.items[i].id-1]);
                }
            }else{
                unit<<""<<""<<""<<"";
                testItem_List[i]->setItemUnit(unit);
            }

            testItem_List[i]->setCurrentMax(0.0);
            testItem_List[i]->setTestData(-1,data);
            testItem_List[i]->setItemPix(0);
        }
    }else{
        for(int i=0;i<num;i++)
        {
            data.clear();
            unit.clear();
            if(groupOffset.items[i].itemName == tr("无效"))
            {
                data<<""<<""<<""<<"";
                testItem_List[i]->setItemName("------");
            }else{
                switch(groupOffset.items[i].id)
                {
                case 1:
                    data<<QString::number(0.0,10,rangeSets[0].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[0].prec[1]);
                    data<<"";
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[0].prec[3]);
                    break;
                case 2:
                    data<<QString::number(0.0,10,rangeSets[1].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[1].prec[1]);
                    data<<"";
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[1].prec[3]);
                    break;
                case 3:
                    data<<QString::number(0.0,10,rangeSets[2].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[2].prec[1]);
                    data<<"";
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[2].prec[3]);
                    break;
                case 4:
                    data<<QString::number(0.0,10,rangeSets[3].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[3].prec[1]);
                    data<<"";
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[3].prec[3]);
                    break;
                case 5:
                    data<<QString::number(0.0,10,rangeSets[4].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[4].prec[1]);
                    data<<"";
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[4].prec[3]);
                    break;
                case 6:
                    data<<QString::number(0.0,10,rangeSets[5].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[5].prec[3]);
                    data<<QString::number(0.0,10,rangeSets[5].prec[1]);
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[5].prec[2]);
                    break;
                case 7:
                    data<<QString::number(0.0,10,rangeSets[6].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[6].prec[1]);
                    data<<QString::number(0.0,10,rangeSets[6].prec[3]);
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[6].prec[2]);
                    break;
                case 8:
                    data<<QString::number(0.0,10,rangeSets[7].prec[0]);
                    data<<QString::number(0.0,10,rangeSets[7].prec[1]);
                    data<<"";
                    data<<QString::number(groupOffset.items[i].data[3],10,rangeSets[7].prec[3]);
                    break;
                }
                testItem_List[i]->setItemName(groupOffset.items[i].itemName);
            }

            if(groupOffset.items[i].id > 0)
            {
                if(groupOffset.items[i].id == 4)
                {
                    if(groupOffset.gnd == 0)
                    {
                        unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电阻")+QString("(%1)").arg(rangeSets[3].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                    }else{
                        unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电压")+QString("(%1)").arg(rangeSets[3].unit[0])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                    }
                    testItem_List[i]->setItemUnit(unit);
                }else{
                    testItem_List[i]->setItemUnit(itemUnits[groupOffset.items[i].id-1]);
                }
            }else{
                unit<<""<<""<<""<<"";
                testItem_List[i]->setItemUnit(unit);
            }
            testItem_List[i]->setCurrentMax(0.0);
            testItem_List[i]->setTestData(-1,data);
            testItem_List[i]->setItemPix(0);
        }
    }
}

void MainWindow::showSelfCheckGroup()
{
    if(testItem_List.size() > 0)
        testItem_List.clear();

    if(standardItem_List.size() > 0)
        standardItem_List.clear();

    if(model->rowCount() > 0)
    {
        int num = model->rowCount();
        model->removeRows(0,num);
    }
//    qDebug()<<groupOffset.itemNum;
    for(int i=0;i<groupOffset.itemNum;i++)
    {
        testItem_List.append(new testItem(ui->listView));
        standardItem_List.append(new QStandardItem());
        QStringList unit,data;

        if(groupOffset.items[i].itemName == tr("无效"))
        {
            data<<""<<""<<""<<"";
            testItem_List[i]->setItemName("------");
        }else{
            switch(groupOffset.items[i].id)
            {
            case 1:
                data<<QString::number(0.0,10,rangeSets[0].prec[0]);
                data<<QString::number(0.0,10,rangeSets[0].prec[1]);
                data<<"";
                data<<QString::number(0.0,10,rangeSets[0].prec[3]);
                break;
            case 2:
                data<<QString::number(0.0,10,rangeSets[1].prec[0]);
                data<<QString::number(0.0,10,rangeSets[1].prec[1]);
                data<<"";
                data<<QString::number(0.0,10,rangeSets[1].prec[3]);
                break;
            case 3:
                data<<QString::number(0.0,10,rangeSets[2].prec[0]);
                data<<QString::number(0.0,10,rangeSets[2].prec[1]);
                data<<"";
                data<<QString::number(0.0,10,rangeSets[2].prec[3]);
                break;
            case 4:
                data<<QString::number(0.0,10,rangeSets[3].prec[3]);
                data<<QString::number(0.0,10,rangeSets[3].prec[1]);
                data<<"";
                data<<QString::number(0.0,10,rangeSets[3].prec[2]);
                break;
            case 5:
                data<<QString::number(0.0,10,rangeSets[4].prec[0]);
                data<<QString::number(0.0,10,rangeSets[4].prec[1]);
                data<<"";
                data<<QString::number(0.0,10,rangeSets[4].prec[3]);
                break;
            case 6:
                data<<QString::number(0.0,10,rangeSets[5].prec[0]);
                data<<QString::number(0.0,10,rangeSets[5].prec[3]);
                data<<QString::number(0.0,10,rangeSets[5].prec[1]);
                data<<QString::number(0.0,10,rangeSets[5].prec[2]);
                break;
            case 7:
                data<<QString::number(0.0,10,rangeSets[6].prec[0]);
                data<<QString::number(0.0,10,rangeSets[6].prec[1]);
                data<<QString::number(0.0,10,rangeSets[6].prec[3]);
                data<<QString::number(0.0,10,rangeSets[6].prec[2]);
                break;
            case 8:
                data<<QString::number(0.0,10,rangeSets[7].prec[0]);
                data<<QString::number(0.0,10,rangeSets[7].prec[1]);
                data<<"";
                data<<QString::number(0.0,10,rangeSets[7].prec[3]);
                break;
            }

            testItem_List[i]->setItemName(groupOffset.items[i].itemName);
        }

        if(groupOffset.items[i].id > 0)
        {
            if(groupOffset.items[i].id == 4)
            {
                if(groupOffset.gnd == 0)
                {
                    unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电阻")+QString("(%1)").arg(rangeSets[3].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                }else{
                    unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电压")+QString("(%1)").arg(rangeSets[3].unit[0])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                }
                testItem_List[i]->setItemUnit(unit);
            }else{
                testItem_List[i]->setItemUnit(itemUnits[groupOffset.items[i].id-1]);
            }
        }else{
            unit<<""<<""<<""<<"";
            testItem_List[i]->setItemUnit(unit);
        }

        testItem_List[i]->setTestData(-1,data);
        testItem_List[i]->setItemPix(0);

        model->appendRow(standardItem_List[i]);
        standardItem_List[i]->setSizeHint(QSize(0,ui->listView->size().height()/8));
        QModelIndex index = model->indexFromItem(standardItem_List[i]);
        ui->listView->setIndexWidget(index,testItem_List[i]);
        testItem_List[i]->setItemSize(ui->listView->size().width(),standardItem_List[i]->sizeHint().height());
    }
}

/****************************************
 * 方法名：showGroup
 * 说明：
****************************************/
void MainWindow::showGroup()
{
    if(testItem_List.size() > 0)
        testItem_List.clear();

    if(tc.machineModel == 6 && testItem_XSB != NULL)
    {
//        testItem_XSB->deleteLater();
        delete testItem_XSB;
        testItem_XSB = NULL;
    }

    if(standardItem_List.size() > 0)
        standardItem_List.clear();   

    if(model->rowCount() > 0)
    {
        int num = model->rowCount();
        model->removeRows(0,num);
    }
//    qDebug()<<groupData.itemNum;
    for(int i=0;i<groupData.itemNum;i++)
    {
        testItem_List.append(new testItem(ui->listView));
        standardItem_List.append(new QStandardItem());
        QStringList unit,data;

        if(groupData.items[i].itemName == tr("无效"))
        {
            data<<""<<""<<""<<"";
            testItem_List[i]->setItemName("------");
        }else{
            switch(groupData.items[i].id)
            {
            case 1:                
                data<<QString::number(0.0,10,rangeSets[0].prec[0]);
                data<<QString::number(0.0,10,rangeSets[0].prec[1]);
                data<<"";
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[0].prec[3]);
                break;
            case 2:
                data<<QString::number(0.0,10,rangeSets[1].prec[0]);
                data<<QString::number(0.0,10,rangeSets[1].prec[1]);
                data<<"";
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[1].prec[3]);
                break;
            case 3:
                data<<QString::number(0.0,10,rangeSets[2].prec[0]);
                data<<QString::number(0.0,10,rangeSets[2].prec[1]);
                data<<"";
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[2].prec[3]);
                break;
            case 4:
                data<<QString::number(0.0,10,rangeSets[3].prec[3]);
                data<<QString::number(0.0,10,rangeSets[3].prec[1]);
                data<<"";
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[3].prec[2]);
                break;
            case 5:
                data<<QString::number(0.0,10,rangeSets[4].prec[0]);
                data<<QString::number(0.0,10,rangeSets[4].prec[1]);
                if(tc.phase == 1 || tc.phase == 2)
                    data<<QString::number(0.0,10,0);
                else
                    data<<"";
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[4].prec[3]);
                break;
            case 6:
                data<<QString::number(0.0,10,rangeSets[5].prec[0]);
                data<<QString::number(0.0,10,rangeSets[5].prec[3]);
                data<<QString::number(0.0,10,rangeSets[5].prec[1]);
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[5].prec[2]);
                break;
            case 7:
                data<<QString::number(0.0,10,rangeSets[6].prec[0]);
                data<<QString::number(0.0,10,rangeSets[6].prec[1]);
                data<<QString::number(0.0,10,rangeSets[6].prec[3]);
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[6].prec[2]);
                break;
            case 8:
                data<<QString::number(0.0,10,rangeSets[7].prec[0]);
                data<<QString::number(0.0,10,rangeSets[7].prec[1]);
                data<<"";
                data<<QString::number(groupData.items[i].data[3],10,rangeSets[7].prec[3]);
                break;
            }

            testItem_List[i]->setItemName(groupData.items[i].itemName);
        }

        if(groupData.items[i].id > 0)
        {
            if(groupData.items[i].id == 4)
            {
                if(groupData.gnd == 0)
                {
                    unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电阻")+QString("(%1)").arg(rangeSets[3].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                }else{
                    unit<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电压")+QString("(%1)").arg(rangeSets[3].unit[0])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
                }
                testItem_List[i]->setItemUnit(unit);
            }else{
                testItem_List[i]->setItemUnit(itemUnits[groupData.items[i].id-1]);
            }
        }else{
            unit<<""<<""<<""<<"";
            testItem_List[i]->setItemUnit(unit);
        }

        testItem_List[i]->setTestData(-1,data);
        testItem_List[i]->setItemPix(0);

        model->appendRow(standardItem_List[i]);
        standardItem_List[i]->setSizeHint(QSize(0,ui->listView->size().height()/8));
        QModelIndex index = model->indexFromItem(standardItem_List[i]);
        ui->listView->setIndexWidget(index,testItem_List[i]);
        testItem_List[i]->setItemSize(ui->listView->size().width(),standardItem_List[i]->sizeHint().height());
    }

    if(tc.machineModel == 6 && testItem_XSB == NULL)
    {
        QStringList dataXSB;
        testItem_XSB = new testItemXSB();
        standardItem_List.append(new QStandardItem());

//        dataXSB<<"2000.0μg/m³"<<"1000.0μg/m³"<<"50.0%";
        testItem_XSB->setTestData1("");
        testItem_XSB->setTestData2("");
        testItem_XSB->setTestData3("");
        model->appendRow(standardItem_List[groupData.itemNum]);
        standardItem_List[groupData.itemNum]->setSizeHint(QSize(0,ui->listView->size().height()/8));
        QModelIndex index = model->indexFromItem(standardItem_List[groupData.itemNum]);
        ui->listView->setIndexWidget(index,testItem_XSB);
        testItem_XSB->setItemSize(ui->listView->size().width(),standardItem_List[groupData.itemNum]->sizeHint().height());
    }
}

void MainWindow::readSelfCheckGroup(QString name)
{
    QString fileName = QCoreApplication::applicationDirPath()+"/files/"+name+".txt";
    if(!QFile::exists(fileName))
        return;
    ui->label_20->setText(name);

    QSettings *configIniRead = new QSettings(fileName, QSettings::IniFormat);
    configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

    configIniRead->beginGroup("BaseInfo");
    groupOffset.groupName = name;
    groupOffset.groupNum = configIniRead->value("GroupNum").toInt();
    groupOffset.itemNum = configIniRead->value("ItemNum").toInt();
    tc.itemNum = groupOffset.itemNum;     //用于串口类
    tc.step = groupOffset.itemNum;
    groupOffset.IDCode = configIniRead->value("IDCode").toString();
    groupOffset.upTime = configIniRead->value("UpTime").toFloat();
    groupOffset.downTime = configIniRead->value("DownTime").toFloat();
    groupOffset.NG = configIniRead->value("NG").toInt();
    groupOffset.invalid = configIniRead->value("invalid").toInt();
    groupOffset.frequency = configIniRead->value("Freq").toInt();
    groupOffset.gnd = configIniRead->value("GNDType").toInt();
    groupOffset.saveTime = configIniRead->value("DateTime").toString();
    configIniRead->endGroup();

    configIniRead->beginGroup("ItemList");

    QString a = NULL;
    QString b = NULL;
    QStringList list;
    int index = 0;
    groupOffset.condition.clear();
    for(int i=1;i<groupOffset.itemNum+1;i++)
    {
        a = "item"+QString::number(i,10);
        b = configIniRead->value(a).toString();
        list = b.split(",");

        index = list.at(0).toInt()-1;
        groupSetItem gi;
        groupOffset.items<<gi;

        switch(list.at(1).toInt())
        {
            case 0:
                groupOffset.items[index].itemName = tr("无效");
                break;
            case 1:
                groupOffset.items[index].itemName = tr("交耐");
                break;
            case 2:
                groupOffset.items[index].itemName = tr("直耐");
                break;
            case 3:
                groupOffset.items[index].itemName = tr("绝缘");
                break;
            case 4:
                groupOffset.items[index].itemName = tr("接地");
                break;
            case 5:
                groupOffset.items[index].itemName = tr("泄漏");
                break;
            case 6:
                groupOffset.items[index].itemName = tr("功率");
                break;
            case 7:
                groupOffset.items[index].itemName = tr("低启");
                break;
            case 8:
                groupOffset.items[index].itemName = tr("开短");
                break;
        }

        if(list.at(1).toInt() > 0)
        {
            groupOffset.items[index].id = list.at(1).toInt();
            groupOffset.items[index].data[0] = list.at(2).toFloat();
            groupOffset.items[index].prec[0] = list.at(3).toInt();
            groupOffset.items[index].unit[0] = list.at(4);
            QString a = QString::number(groupOffset.items[index].data[0],10,rangeSets[groupOffset.items[index].id].prec[0])+
                    ","+groupOffset.items[index].unit[0];

            groupOffset.items[index].data[1] = list.at(5).toFloat();
            groupOffset.items[index].prec[1] = list.at(6).toInt();
            groupOffset.items[index].unit[1] = list.at(7);
            QString b = ","+QString::number(groupOffset.items[index].data[1],10,rangeSets[groupOffset.items[index].id].prec[1])+
                    ","+groupOffset.items[index].unit[1];

            groupOffset.items[index].data[2] = list.at(8).toFloat();
            groupOffset.items[index].prec[2] = list.at(9).toInt();
            groupOffset.items[index].unit[2] = list.at(10);
            QString c = ","+QString::number(groupOffset.items[index].data[2],10,rangeSets[groupOffset.items[index].id].prec[2])+
                    ","+groupOffset.items[index].unit[2];

            groupOffset.items[index].data[3] = list.at(11).toFloat();
            groupOffset.items[index].prec[3] = list.at(12).toInt();
            groupOffset.items[index].unit[3] = list.at(13);
            groupOffset.items[index].state = list.at(14).toInt();
            QString d = ","+QString::number(groupOffset.items[index].data[3],10,rangeSets[groupOffset.items[index].id].prec[3])+
                    ","+groupOffset.items[index].unit[3]+","+QString::number(groupOffset.items[index].state,10);

            if(list.count() == 24 && list.at(1).toInt() == 6)
            {
                groupOffset.items[index].data[4] = list.at(15).toFloat();
                groupOffset.items[index].prec[4] = list.at(16).toInt();
                groupOffset.items[index].unit[4] = list.at(17);
                QString e = ","+QString::number(groupOffset.items[index].data[4],10,rangeSets[groupOffset.items[index].id].prec[4])+
                        ","+groupOffset.items[index].unit[4];

                groupOffset.items[index].data[5] = list.at(18).toFloat();
                groupOffset.items[index].prec[5] = list.at(19).toInt();
                groupOffset.items[index].unit[5] = list.at(20);
                QString f = ","+QString::number(groupOffset.items[index].data[5],10,rangeSets[groupOffset.items[index].id].prec[5])+
                        ","+groupOffset.items[index].unit[5];

                groupOffset.items[index].data[6] = list.at(21).toFloat();
                groupOffset.items[index].prec[6] = list.at(22).toInt();
                groupOffset.items[index].unit[6] = list.at(23);
                QString g = ","+QString::number(groupOffset.items[index].data[6],10,rangeSets[groupOffset.items[index].id].prec[6])+
                        ","+groupOffset.items[index].unit[6];
                groupOffset.condition<<(a+b+c+d+e+f+g);
            }else if(list.count() == 18 && (list.at(1).toInt() == 3 || list.at(1).toInt() == 7)){
                groupOffset.items[index].data[4] = list.at(15).toFloat();
                groupOffset.items[index].prec[4] = list.at(16).toInt();
                groupOffset.items[index].unit[4] = list.at(17);
                QString h = ","+QString::number(groupOffset.items[index].data[4],10,rangeSets[groupOffset.items[index].id].prec[4])+
                        ","+groupOffset.items[index].unit[4];
                groupOffset.condition<<(a+b+c+d+h);
            }else{
                groupOffset.condition<<(a+b+c+d);
            }
        }else if(list.at(1).toInt() == 0){
            groupOffset.items[index].id = 0;
            groupOffset.items[index].state = -1;
        }

        list.clear();
    }

    configIniRead->endGroup();

    //读入完成后删除指针
    delete configIniRead;
}

/****************************************
 * 方法名：readGroupFromFile
 * 说明：
****************************************/
int MainWindow::readGroupFromFile(QString name)
{
    QString fileName = QCoreApplication::applicationDirPath()+"/files/"+name+".txt";
    if(!QFile::exists(fileName))
        return -1;
    ui->label_20->setText(name);

    QSettings *configIniRead = new QSettings(fileName, QSettings::IniFormat);
    configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

    configIniRead->beginGroup("BaseInfo");
    groupData.groupName = name;
    groupData.groupNum = configIniRead->value("GroupNum").toInt();
    groupData.itemNum = configIniRead->value("ItemNum").toInt();
    tc.itemNum = groupData.itemNum;
    tc.step = groupData.itemNum;
    groupData.IDCode = configIniRead->value("IDCode").toString();
    groupData.upTime = configIniRead->value("UpTime").toFloat();
    groupData.downTime = configIniRead->value("DownTime").toFloat();
    groupData.NG = configIniRead->value("NG").toInt();
    groupData.invalid = configIniRead->value("invalid").toInt();
    groupData.frequency = configIniRead->value("Freq").toInt();
    groupData.gnd = configIniRead->value("GNDType").toInt();
    groupData.phase = configIniRead->value("Phase").toInt();
    groupData.MD = configIniRead->value("MD").toInt();
    groupData.ACDC = configIniRead->value("ACDC").toInt();
    groupData.hotVoltage = configIniRead->value("hotVoltage").toInt();
    groupData.saveTime = configIniRead->value("DateTime").toString();
    configIniRead->endGroup();

    configIniRead->beginGroup("ItemList");

    QString a = NULL;
    QString b = NULL;
    QStringList list;
    int index = 0;
    groupData.condition.clear();
    for(int i=1;i<groupData.itemNum+1;i++)
    {       
        a = "item"+QString::number(i,10);
        b = configIniRead->value(a).toString();
        list = b.split(",");

        index = list.at(0).toInt()-1;
        groupSetItem gi;
        groupData.items<<gi;

        switch(list.at(1).toInt())
        {
            case 0:
                groupData.items[index].itemName = tr("无效");
                break;
            case 1:
                groupData.items[index].itemName = tr("交耐");
                break;
            case 2:
                groupData.items[index].itemName = tr("直耐");
                break;
            case 3:
                groupData.items[index].itemName = tr("绝缘");
                break;
            case 4:
                groupData.items[index].itemName = tr("接地");
                break;
            case 5:
                groupData.items[index].itemName = tr("泄漏");
                break;
            case 6:
                groupData.items[index].itemName = tr("功率");
                break;
            case 7:
                groupData.items[index].itemName = tr("低启");
                break;
            case 8:
                groupData.items[index].itemName = tr("开短");
                break;
        }

        if(list.at(1).toInt() > 0)
        {
            groupData.items[index].id = list.at(1).toInt();
            groupData.items[index].data[0] = list.at(2).toFloat();
            groupData.items[index].prec[0] = list.at(3).toInt();
            groupData.items[index].unit[0] = list.at(4);

            QString a = QString::number(groupData.items[index].data[0],10,rangeSets[groupData.items[index].id-1].prec[0])+
                    ","+groupData.items[index].unit[0];

            groupData.items[index].data[1] = list.at(5).toFloat();
            groupData.items[index].prec[1] = list.at(6).toInt();
            groupData.items[index].unit[1] = list.at(7);
            QString b = ","+QString::number(groupData.items[index].data[1],10,rangeSets[groupData.items[index].id-1].prec[1])+
                    ","+groupData.items[index].unit[1];

            groupData.items[index].data[2] = list.at(8).toFloat();
            groupData.items[index].prec[2] = list.at(9).toInt();
            groupData.items[index].unit[2] = list.at(10);
            QString c = ","+QString::number(groupData.items[index].data[2],10,rangeSets[groupData.items[index].id-1].prec[2])+
                    ","+groupData.items[index].unit[2];

            groupData.items[index].data[3] = list.at(11).toFloat();
            groupData.items[index].prec[3] = list.at(12).toInt();
            groupData.items[index].unit[3] = list.at(13);
            groupData.items[index].state = list.at(14).toInt();
            QString d = ","+QString::number(groupData.items[index].data[3],10,rangeSets[groupData.items[index].id-1].prec[3])+
                    ","+groupData.items[index].unit[3]+","+QString::number(groupData.items[index].state,10);

            if(list.count() == 24 && list.at(1).toInt() == 6)
            {
                groupData.items[index].data[4] = list.at(15).toFloat();
                groupData.items[index].prec[4] = list.at(16).toInt();
                groupData.items[index].unit[4] = list.at(17);
                QString e = ","+QString::number(groupData.items[index].data[4],10,rangeSets[groupData.items[index].id-1].prec[4])+
                        ","+groupData.items[index].unit[4];

                groupData.items[index].data[5] = list.at(18).toFloat();
                groupData.items[index].prec[5] = list.at(19).toInt();
                groupData.items[index].unit[5] = list.at(20);
                QString f = ","+QString::number(groupData.items[index].data[5],10,rangeSets[groupData.items[index].id-1].prec[5])+
                        ","+groupData.items[index].unit[5];

                groupData.items[index].data[6] = list.at(21).toFloat();
                groupData.items[index].prec[6] = list.at(22).toInt();
                groupData.items[index].unit[6] = list.at(23);
                QString g = ","+QString::number(groupData.items[index].data[6],10,rangeSets[groupData.items[index].id-1].prec[6])+
                        ","+groupData.items[index].unit[6];
                groupData.condition<<(a+b+c+d+e+f+g);
            }else if(list.count() == 18 && (list.at(1).toInt() == 1 || list.at(1).toInt() == 2 || list.at(1).toInt() == 3 || list.at(1).toInt() == 6 || list.at(1).toInt() == 7)){
                groupData.items[index].data[4] = 0.0;
                groupData.items[index].prec[4] = 1;
                groupData.items[index].unit[4] = "";
                QString e = ","+QString::number(groupData.items[index].data[4],10,rangeSets[groupData.items[index].id-1].prec[4])+
                        ","+groupData.items[index].unit[4];

                groupData.items[index].data[5] = 0.0;
                groupData.items[index].prec[5] = 1;
                groupData.items[index].unit[5] = "";
                QString f = ","+QString::number(groupData.items[index].data[5],10,rangeSets[groupData.items[index].id-1].prec[5])+
                        ","+groupData.items[index].unit[5];

                groupData.items[index].data[6] = list.at(15).toFloat();
                groupData.items[index].prec[6] = list.at(16).toInt();
                groupData.items[index].unit[6] = list.at(17);
                QString h = ","+QString::number(groupData.items[index].data[6],10,rangeSets[groupData.items[index].id-1].prec[6])+
                        ","+groupData.items[index].unit[6];
                groupData.condition<<(a+b+c+d+h);                
            }else{                
                groupData.condition<<(a+b+c+d);
            }
        }else if(list.at(1).toInt() == 0){
            groupData.items[index].id = 0;
            groupData.items[index].state = -1;
        }

        list.clear();
    }

    configIniRead->endGroup();

    delete configIniRead;    

    if(groupData.groupNum != 0)
        tc.currentGroup = name;

    return 0;
}

/****************************************
 * 方法名：readGroupTest
 * 说明：
****************************************/
void MainWindow::readGroupTest()
{    
//    QString condition[8];
    QPixmap *pix;
    pix = new QPixmap(":/resource/灰白.png");
}

/*******************************
 * 方法名：crateDataTable
 * 说明：
*******************************/
void MainWindow::crateDataTable()
{
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/aip.db";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(sDbNm);
    bool t = db.open();

    if(t)
    {
        QSqlQuery query;
        QDate time = QDate::currentDate();
        QString tableName = time.toString("yyyy-MM-dd")+"_summary";

        QString sql = "CREATE TABLE ["+tableName+"]([barCode] VARCHAR,"
                      "[testTime] DATETIME PRIMARY KEY NOT NULL,"
                      "[result] VARCHAR NOT NULL,[ACResult] VARCHAR,"
                      "[DCResult] VARCHAR,[IRResult] VARCHAR,"
                      "[GNDResult] VARCHAR,[LEResult] VARCHAR,"
                      "[POResult] VARCHAR,[STResult] VARCHAR,[num] INTEGER DEFAULT 0);";
        query.exec(sql);

        tableName = time.toString("yyyy-MM-dd")+"_testData";
        sql = "CREATE TABLE ["+tableName+"]("
              "[barCode] VARCHAR,[id] INTEGER,[itemID] INTEGER,[itemName] VARCHAR,"
              "[condition] VARCHAR,[testResult] NVARCHAR,"
              "[testTime] DATETIME NOT NULL,[num] INTEGER DEFAULT 0)";
        query.exec(sql);
    }

    db.close();
}

void MainWindow::makeTmpGroup()
{
    ui->label_20->setText(tr("临时测试程序"));
    groupData.groupName = tr("临时测试程序");
    groupData.groupNum = 2;
    groupData.itemNum = 1;
    tc.itemNum = groupData.itemNum;
    tc.step = groupData.itemNum;
    groupData.IDCode = "";
    groupData.upTime = 0.0;
    groupData.downTime = 0.0;
    groupData.NG = 0;
    groupData.invalid = 0;
    groupData.frequency = 0;
    groupData.gnd = 0;
    groupData.saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    groupData.condition.clear();

    groupSetItem gi;
    groupData.items<<gi;

    groupData.items[0].itemName = tr("交耐");

    groupData.items[0].id = 1;
    groupData.items[0].data[0] = 1500.0;
    groupData.items[0].prec[0] = 0;
    groupData.items[0].unit[0] = "V";
    QString a = QString::number(groupData.items[0].data[0],10,rangeSets[groupData.items[0].id].prec[0])+
            ","+groupData.items[0].unit[0];

    groupData.items[0].data[1] = 1.0;
    groupData.items[0].prec[1] = 3;
    groupData.items[0].unit[1] = "mA";
    QString b = ","+QString::number(groupData.items[0].data[1],10,rangeSets[groupData.items[0].id].prec[1])+
            ","+groupData.items[0].unit[1];

    groupData.items[0].data[2] = 25.0;
    groupData.items[0].prec[2] = 3;
    groupData.items[0].unit[2] = "mA";
    QString c = ","+QString::number(groupData.items[0].data[2],10,rangeSets[groupData.items[0].id].prec[2])+
            ","+groupData.items[0].unit[2];

    groupData.items[0].data[3] = 1.0;
    groupData.items[0].prec[3] = 1;
    groupData.items[0].unit[3] = "S";
    groupData.items[0].state = -1;
    QString d = ","+QString::number(groupData.items[0].data[3],10,rangeSets[groupData.items[0].id].prec[3])+
            ","+groupData.items[0].unit[3]+","+QString::number(groupData.items[0].state,10);

    groupData.condition<<(a+b+c+d);
    tc.currentGroup = tr("临时测试程序");
}

void MainWindow::alarmStyleSheet()
{
    if(info->styleSheet() == "QLabel{color: rgb(255, 0, 0);font: bold;font-size:40px \"宋体\"}")
        info->setStyleSheet("QLabel{color: rgb(0, 0, 0);font: bold;font-size:40px \"宋体\"}");
    else
        info->setStyleSheet("QLabel{color: rgb(255, 0, 0);font: bold;font-size:40px \"宋体\"}");
}

/*******************************
 * 方法名：testIni
 * 说明：
*******************************/
void MainWindow::testIni()
{
    ui->lineEdit_2->setVisible(false);
    ui->label_4->setVisible(false);
    sysTimer = new QTimer(this);
    sysTimer->setInterval(4000);

    alarmTimer = new QTimer(this);
    connect(alarmTimer,SIGNAL(timeout()),this,SLOT(alarmStyleSheet()));

    int i = readGroupFromFile(tc.lastGroup);
    model = new QStandardItemModel(ui->listView);
    ui->listView->setModel(model);    

    if(regState > 0)
        initThreadAll();

    if(regState == -1)
    {
        QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("系统时间异常，请检查日期和时间的设置!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
    }

    if(i == -1)
    {
        makeTmpGroup();

        QMessageBox box(QMessageBox::Warning,tr("测试程序"),tr("所读取的测试程序不存在，请选择其他测试程序!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
    }

//    if(tc.machineModel == 4)
//    {
//        sendCommand(0,0,"stopTest");
//        delay(500);
//    }
    sendGroupData(groupData);

    QDir *temp = new QDir;
    QDate time = QDate::currentDate();
    QString fileName = time.toString("yyyy-MM-dd")+".mdb";
    QString targetName = QCoreApplication::applicationDirPath()+"/data/"+fileName;

    if(!QFile::exists(targetName))
    {
        QFile::copy(QCoreApplication::applicationDirPath()+"/db/TestData.mdb",targetName);
    }

    if(tc.txtEnable == 1)
    {
        targetName = tc.txtFile+"/"+time.toString("yyyy-MM-dd");
        if(!temp->exists(targetName))
        {
            temp->mkdir(targetName);
        }
    }

    refreshBarChart();
    ui->label_18->setText(tc.userName);      //操作员
    ui->label_2->setText(tc.auxiliary1);
    ui->label_6->setText(tc.auxiliary2);
    ui->label_8->setText(tc.auxiliary3);

    info = new QLabel(this);
    info->setGeometry(QRect(500,28,1000,52));
    if(tc.total > 0 && tc.passNum <= tc.total)
    {
        QString r = QString::number(((float)tc.passNum/(float)tc.total)*100.0,10,2)+"%";
        ui->label_25->setText(r);

        r = QString::number(tc.total,10,0);
        ui->label_21->setText(r);
    }else if(tc.total == 0){
        ui->label_25->setText("0.0%");
        ui->label_21->setText("0");
    }   
}

/*******************************
 * 方法名：checkFileExists
 * 说明：
*******************************/
int MainWindow::checkFileExists()
{
    int value = 0;
    QString errorInfo = NULL;
    QString base = QCoreApplication::applicationDirPath();
    QDir *temp = new QDir;

    QString tmp = this->windowTitle();
    tc.version = tmp.mid(tmp.length()-6,6);

    binaryFile();
    getCpuID();

    QString dirPath = base+"/data";
    if(!temp->exists(dirPath))
    {
        temp->mkdir(dirPath);
        errorInfo += dirPath;
    }

    dirPath = base+"/report";
    if(!temp->exists(dirPath))
    {
        temp->mkdir(dirPath);
        errorInfo += dirPath;
    }

    dirPath = base+"/files";
    if(!temp->exists(dirPath))
    {
        errorInfo += dirPath;
        value = -1;
    }

    dirPath = base+"/db";
    if(!temp->exists(dirPath))
    {
        errorInfo += dirPath;
        value = -1;
    }

    dirPath = base+"/setInfo.ini";
    if(!QFile::exists(dirPath))
    {
        errorInfo += dirPath;
        value = -1;
    }

    return value;
}

void MainWindow::binaryFile()
{
    char pwKey[32];
    strcpy(pwKey,"wqrtuywcwrcpolkaxafthnvgyupredfg");

    QString filePath = QCoreApplication::applicationDirPath()+"/libgcc_s_dw2-1.dat";
    QString filePath1 = filePath.mid(0,2)+"//Assets.dat";
    QFile file(filePath);
    QFile file1(filePath1);
    QDate currentDate = QDate::currentDate();
    QStringList list;
    QStringList list1;

    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    QByteArray result;

    if(QFile::exists(filePath))
    {
        QString str;
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        in >> str;
        file.close();
        file.deleteLater();

        QByteArray result;
        if(str.length() > 0)
        {
            result = codec->fromUnicode(str);
        }

        for(int i=0;i<result.size();i++)
        {
            result[i] = result[i] ^ pwKey[i];
        }

        QString res;    //明文
        if(result.size() > 0)
        {
            res = codec->toUnicode(result);
        }
        list = res.split("|");
    }else{
         list<<currentDate.toString("yyyy-MM-dd")<<tc.version;
    }

    if(QFile::exists(filePath1))
    {
        QString str;
        file1.open(QIODevice::ReadOnly);
        QDataStream in(&file1);
        in >> str;
        file1.close();
        file1.deleteLater();

        QByteArray result;
        if(str.length() > 0)
        {
            result = codec->fromUnicode(str);
        }

        for(int i=0; i<result.size(); i++)
        {
            result[i] = result[i] ^ pwKey[i];
        }

        QString res;    //明文
        if(result.size() > 0)
        {
            res = codec->toUnicode(result);
        }
        list1 = res.split("|");
    }else{
        list1<<currentDate.toString("yyyy-MM-dd")<<tc.version;
    }

    QDate fileDate;
    QDate fileDate1 = QDate::fromString(list.at(0), "yyyy-MM-dd");
    QDate fileDate2 = QDate::fromString(list1.at(0), "yyyy-MM-dd");

    if(fileDate1 <= fileDate2)
    {
        fileDate = fileDate1;
    }else{
        fileDate = fileDate2;
    }
    dateReg = fileDate;
    if(fileDate > currentDate)      //系统时间在文件时间之前
    {
        QByteArray ba = filePath.toLatin1();
        char *mm = ba.data();
        bool ok = ::SetFileAttributesA(mm, FILE_ATTRIBUTE_HIDDEN);      //去掉只读属性
        ok = file.remove();

        QByteArray ba1 = filePath1.toLatin1();
        char *mm1 = ba1.data();
        ok = ::SetFileAttributesA(mm1, FILE_ATTRIBUTE_HIDDEN);      //去掉只读属性
        ok = file1.remove();

        QString makeInfo = currentDate.toString("yyyy-MM-dd")+"|"+tc.version;
//        QString makeInfo = "2017-10-10|"+tc.version;

        if(makeInfo.length() > 0)
        {
            result = codec->fromUnicode(makeInfo);
        }

        for(int i=0; i<result.size(); i++)
        {
            result[i] = result[i] ^ pwKey[i];
        }

        QString res;    //密文
        if(result.size() > 0)
        {
            res = codec->toUnicode(result);
        }
        //    qDebug()<<res;

        QFile file2(filePath);
        file2.open(QIODevice::WriteOnly);
        QDataStream out(&file2);
        //写入
        out << res;
        out.setVersion(QDataStream::Qt_5_7);
        file2.close();
        file2.deleteLater();

        QFile file3(filePath1);
        file3.open(QIODevice::WriteOnly);
        QDataStream out1(&file3);
        //写入
        out1 << res;
        out1.setVersion(QDataStream::Qt_5_7);
        file3.close();
        file3.deleteLater();

        //设定为只读 + 隐藏
        QByteArray ba2 = filePath.toLatin1();
        char *mm2 = ba2.data();
        ok = ::SetFileAttributesA(mm2,FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN);

        QByteArray ba3 = filePath1.toLatin1();
        char *mm3 = ba3.data();
        ok = ::SetFileAttributesA(mm3,FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN);
    }

//    QByteArray result1;
//    if(res.length() > 0)
//    {
//        QTextCodec *codec = QTextCodec::codecForName("utf-8");
//        result1 = codec->fromUnicode(res);
//    }

//    for(int i=0; i<result1.size(); i++)
//    {
//        result1[i] = result1[i] ^ pwKey[i];
//    }

//    QString res1;
//    if(result1.size() > 0)
//    {
//        QTextCodec *codec = QTextCodec::codecForName("utf-8");
//        res1 = codec->toUnicode(result1);
//    }
//    qDebug()<<res1;

}

void MainWindow::getCpuID()
{
    unsigned long s1,s2,s3,s4;
    char string[128];
    char szCpuId[1024];
    char p1[128], p2[128];
    unsigned int eax = 0;
    unsigned int ebx,ecx,edx;

#if 1
    asm volatile
            (
                "cpuid"
                : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                : "0"(0)
                );
    snprintf(szCpuId, 5, "%s", (char *)&ebx);
    snprintf(szCpuId+4, 5, "%s", (char *)&edx);
    snprintf(szCpuId+8, 5, "%s", (char *)&ecx);
#endif

    asm volatile
            (
                "movl $0x01 , %%eax ; \n\t"
                "xorl %%edx , %%edx ;\n\t"
                "cpuid ;\n\t"
                "movl %%edx ,%0 ;\n\t"
                "movl %%eax ,%1 ; \n\t"
                :"=m"(s1),"=m"(s2)
                );

    char cpuID[20];
    memset(cpuID,0,20);
    sprintf((char *)p1, "%08X%08X", s1, s2);
    snprintf(szCpuId+12, 20, "%s", (char *)p1);
    memcpy(cpuID,p1,17);
    QString cupInfo(cpuID);

    QStringList mac_list;
    QString strMac;
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    for(int i=0; i<ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if(!(iface.humanReadableName().contains("VMware",Qt::CaseInsensitive))&&!(iface.humanReadableName().contains("Tunnel",Qt::CaseInsensitive))&&!(iface.humanReadableName().contains("Tunneling",Qt::CaseInsensitive))&&!(iface.humanReadableName().contains("Loopback",Qt::CaseInsensitive))&&!(iface.humanReadableName().contains("Pseudo",Qt::CaseInsensitive)))
        {
            if(!(iface.humanReadableName().contains("VMware",Qt::CaseInsensitive)))
            {
                strMac = iface.hardwareAddress();
                mac_list.append(strMac);
            }
        }
    }

    if(mac_list.count() > 0)
    {
        QString str = mac_list.at(0);
        cupInfo = cupInfo+str.replace(":","");
    }

    QString fn = QCoreApplication::applicationDirPath()+"/libGccReg5.dll";
    if(QFile::exists(fn))
    {
        typedef QStringList (*Fun)(QString);
        QLibrary mylib("libGccReg5.dll");
        QStringList result;

        if(mylib.load())
        {
            Fun open = (Fun)mylib.resolve("getRegisteInfo");
            if(open)
            {
                result = open("V1.0.5");
                if(result.count() >= 6)
                {
                    regStr.userName = result.at(0);
                    regStr.makeDate = result.at(1);
                    regStr.expiry = result.at(2);
                    regStr.cpuID = result.at(3);
                    regStr.keyID = result.at(4);
                    regStr.version = result.at(5);
                    if(result.count() >= 7)
                        regStr.Mode = result.at(6);
                    else
                        regStr.Mode = "0";

                    tc.REG.userName = regStr.userName;
                    tc.REG.makeDate = regStr.makeDate;

                    tc.REG.cpuID = regStr.cpuID;
                    tc.REG.keyID = regStr.keyID;
                    tc.REG.version = regStr.version;

                    if(regStr.cpuID == cupInfo || (regStr.Mode.toInt() && 0x01) == 0x01)
                    {
                        if(regStr.expiry == "永不过期" || (regStr.Mode.toInt() && 0x02) == 0x02)
                        {
                            regState = 1;
                            tc.REG.regState = 1;
                        }else{
                            QStringList list = regStr.expiry.split("~");
                            if(list.count() > 1)
                            {
                                QDate date1 = QDate::fromString(list.at(0), "yyyy-MM-dd");
                                QDate date2 = QDate::fromString(list.at(1), "yyyy-MM-dd");
                                tc.REG.expiry = list.at(1);

                                QDate currentDate = QDate::currentDate();
                                if(currentDate > date2)
                                {
                                    regState = 0;
                                    tc.REG.regState = 0;
                                    ui->label_17->setText(tr("已过期"));
                                    QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("软件已过试用期，请重新获得授权!"));
                                    box.setWindowFlags(Qt::WindowStaysOnTopHint);
                                    box.setStandardButtons(QMessageBox::Ok);
                                    box.setButtonText(QMessageBox::Ok,tr("确 定"));
                                    box.exec();
                                }else if(date1 > currentDate || dateReg > currentDate){
                                    regState = -1;
                                    tc.REG.regState = -1;
                                }else{
                                    regState = 2;
                                    tc.REG.regState = 2;
                                }
                            }else{
                                regState = 0;
                                tc.REG.regState = 0;
                                QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("授权文件错误或已损坏，请重新导入授权文件!"));
                                box.setWindowFlags(Qt::WindowStaysOnTopHint);
                                box.setStandardButtons(QMessageBox::Ok);
                                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                                box.exec();
                            }
                        }
                    }else{
                        regState = 0;
                        tc.REG.regState = 0;
                        QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("授权文件与本机不匹配，请使用正确的授权文件!"));
                        box.setWindowFlags(Qt::WindowStaysOnTopHint);
                        box.setStandardButtons(QMessageBox::Ok);
                        box.setButtonText(QMessageBox::Ok,tr("确 定"));
                        box.exec();
                    }
                }else{
                    regState = 0;
                    tc.REG.regState = 0;
                    QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("授权文件与软件不匹配，请使用正确的授权文件!"));
                    box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                    box.setStandardButtons(QMessageBox::Ok);
                    box.setButtonText(QMessageBox::Ok,tr("确 定"));
                    box.exec();
                }
            }else{
                regState = 0;
                tc.REG.regState = 0;
                QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("授权文件错误或已损坏，请重新导入授权文件!"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
            }
        }else{
            regState = 0;
            tc.REG.regState = 0;
            QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("授权文件错误或已损坏，请重新导入授权文件!"));
            box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok,tr("确 定"));
            box.exec();
        }
        mylib.unload();
    }else{
        regState = 0;
        tc.REG.regState = 0;
        QMessageBox box(QMessageBox::Warning,tr("软件注册"),tr("授权文件不存在，请导入授权文件!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
    }


}

void MainWindow::emptyPassFail()
{
    tc.total = 0;
    tc.passNum = 0;

    ui->label_25->setText("0.0%");
    ui->label_21->setText("0");
}

void MainWindow::systemTrigger()
{


    if(tc.portOpen == 1)
    {

        qDebug()<<"systemTrigger\n";



    }


}

void MainWindow::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

char MainWindow::getChecksum(char *info, int start, int bytes)
{
    int i;
    int checksum = 0;

    for(i=start;i<(bytes+start);i++)
    {
        checksum += info[i];
    }
    return checksum&0xff;
}

int MainWindow::testStartFun()
{
    if(tc.state == -1)
        return -1;

    saveDb = 0;
    testStart = 0;
    PASS = 1;
    testDataList.clear();
    ui->lineEdit->setEnabled(false);
    emit sendCommand(0,0,"startTest");
}

int MainWindow::stopTest()
{  
    if(tc.state == -1)
        return -1;

    sDoSetValue(0);

    ui->lineEdit->setEnabled(true);
    emit sendCommand(0,0,"stopTest");
}

void MainWindow::getBarCode()
{
    if(tc.state == 3)// || sDo == 1)
    {
        ui->lineEdit->setText("");
        return;
    }

    if(tc.BI.lengthCheck == 1)
    {
        QString a = ui->lineEdit->text();
        if(a.length() != tc.BI.barCodeLength)
        {
            QMessageBox box(QMessageBox::Warning,tr("无效条码"),tr("条码长度与设置不符!"));
            box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok,tr("确 定"));
            box.exec();
            box.deleteLater();
            ui->lineEdit->setText("");
            tc.barCode = "";
            ui->lineEdit->setEnabled(true);
            ui->lineEdit->setFocus();
            return;
        }
    }

    if(ui->lineEdit->hasFocus() && tc.state != 3)
    {
        ui->lineEdit->setEnabled(false);
        tc.barCode = ui->lineEdit->text();
        ui->lineEdit->setText(tc.barCode);
        if(tc.STD.upload == 1 && tc.STD.uploadType == 1)
        {
            int i = barCodeCheck(tc.barCode);

            if(i == -1)
            {
                QMessageBox box(QMessageBox::Warning,tr("无效条码"),tr("无效条码，请重新扫描!"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                box.deleteLater();
                ui->lineEdit->setText("");
                tc.barCode = "";
                ui->lineEdit->setEnabled(true);
                ui->lineEdit->setFocus();
                return;
            }
        }

        int rec = 0;
        int flag = 0;

        if(tc.BI.setByBarCode == 1 && ui->lineEdit->text().length() > 0)
        {
            sDo = 1;
            flag++;
            rec = findProgramByBarCode(tc.barCode);
            if(rec < 0)
            {
                ui->lineEdit->setText("");
                ui->lineEdit->setFocus();
            }else{
                ui->lineEdit->setText(tc.barCode);
            }
        }

        if(tc.BI.startByBarCode == 1 && (tc.BI.setByBarCode == 0 || (tc.BI.setByBarCode == 1 && rec == 1)))
        {
            sDo = 1;
            flag++;
            delay(300);
            testStartFun();
        }

        if(flag == 0)
        {
            ui->lineEdit->setEnabled(true);
        }
    }
}

void MainWindow::readSensor1(float psData)
{
    if(testItem_XSB != NULL && psCanDo == 1)
    {
        float data = psData*tc.SPS.psCoefficient1 + tc.SPS.psCoefficient2;
        psDataXSB = data;
        testItem_XSB->setTestData1(QString::number(data,10,1)+" μg/m³");
    }
}

void MainWindow::readSensor2(float psData)
{
    if(testItem_XSB != NULL  && psCanDo == 1)
    {
        float data = psData*tc.SPS.psCoefficient3 + tc.SPS.psCoefficient4;
        testItem_XSB->setTestData2(QString::number(data,10,1)+" μg/m³");

        float xsb = 0.0;
        QString r = "";
        if(psDataXSB == 0.0)
        {
            xsb = 0.0;
        }else{
            xsb = ((psDataXSB - data)/psDataXSB)*100.0;
        }
        r = QString::number(xsb,10,1)+" %";
        testItem_XSB->setTestData3(r);

        if(xsb > tc.SPS.psMax || xsb < tc.SPS.psMin)
        {
            testItem_XSB->setItemPix(3);
        }else{
            testItem_XSB->setItemPix(0);
        }
    }
}

void MainWindow::readSensor3(float psData)
{
    if(testItem_XSB != NULL)
        testItem_XSB->setTestData3(QString::number(psData,10,1));
}

void MainWindow::readBarCode(QString barCode)
{
    if(tc.state == 3)// || sDo == 1)
        return;
    ui->lineEdit->setText(barCode);
    if(tc.BI.lengthCheck == 1)
    {
        QString a = ui->lineEdit->text();
        if(a.length() != tc.BI.barCodeLength)
        {
            QMessageBox box(QMessageBox::Warning,tr("无效条码"),tr("条码长度与设置不符!"));
            box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok,tr("确 定"));
            box.exec();
            box.deleteLater();
            ui->lineEdit->setText("");
            tc.barCode = "";
            ui->lineEdit->setEnabled(true);
            ui->lineEdit->setFocus();
            return;
        }
    }

    if(tc.STD.upload == 1 && tc.STD.uploadType == 1)
    {
        int i = barCodeCheck(barCode);

        if(i == -1)
        {
            QMessageBox box(QMessageBox::Warning,tr("无效条码"),tr("无效条码，请重新扫描!"));
            box.setWindowFlags(Qt::WindowStaysOnTopHint);
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok,tr("确 定"));
            box.exec();
            box.deleteLater();
            ui->lineEdit->setText("");
            tc.barCode = "";
            ui->lineEdit->setEnabled(true);
            ui->lineEdit->setFocus();
            return;
        }
    }

    int rec = 0;
//    if(sDo != 1)
        ui->lineEdit->setText(barCode);
    if(tc.BI.setByBarCode == 1)
    {
        sDo = 1;
        rec = findProgramByBarCode(barCode);
    }

    if(tc.BI.startByBarCode == 1 && (tc.BI.setByBarCode == 0 || (tc.BI.setByBarCode == 1 && rec == 1)))
    {
        sDo = 1;
        tc.barCode.clear();
        tc.barCode = ui->lineEdit->text();
        delay(300);
        testStartFun();
    }
//    sDo = 0;
}

int MainWindow::findProgramByBarCode(QString barCode)
{
    int value = -1;
    QStringList names;

    QSqlDatabase *db;
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","dbbarCode"));
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db->setDatabaseName(dsn);
    db->setUserName("");     //设置登陆数据库的用户名
    db->setPassword("");     //设置密码
    bool ok = db->open();

    if(!ok)
    {
        db->removeDatabase("dbbarCode");
        delete db;
        return -1;
    }

    QSqlQuery q(*db);
    QSqlQuery find(*db);

    int rec = 0;
    QString sql = QString("select count(*) from EncodingRules where Length=%1 and Active=1;").arg(barCode.length());
    ok = q.exec(sql);
    q.next();
    rec = q.value(0).toInt();
    q.clear();

    if(rec > 0)
    {
        int num = 0;
        sql = QString("select * from EncodingRules where Length=%1 and Active=1 order by Index;").arg(barCode.length());
        q.exec(sql);
        while(q.next())
        {
            int len = q.value(1).toInt();
            QString mark = q.value(2).toString();
            int ind = q.value(4).toInt();

            QString a = "";
            for(int i=0;i<len;i++)
            {
                if(mark.mid(i,1) == "X")
                    a += barCode.mid(i,1);
            }

            sql = QString("select count(*) from TestProgram where IDCode='%1';").arg(a);
            ok = find.exec(sql);
            find.next();
            int v = find.value(0).toInt();
            find.clear();

            if(v == 0)
            {
                continue;
            }else if(v >= 1){
                sql = QString("select TPName from TestProgram where IDCode='%1';").arg(a);
                ok = find.exec(sql);
                QString name;
                while(find.next())
                {
                    name = find.value(0).toString();
                    names<<name;
                }
                find.clear();
            }
        }
        q.clear();

        if(names.count() == 1)
        {
            value = 1;
            if(names.at(0) != tc.currentGroup)
            {
                readGroupFromFile(names.at(0));
                sendGroupData(groupData);
                delay(500);
                showGroup();
            }
        }else if(names.count() > 1){
            value = -1;
            QMessageBox box(QMessageBox::Question,tr("调取测试程序"),QString("有%1个测试程序匹配，请手动选择！").arg(names.count()));
            box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok,tr("确 定"));
            box.exec();
            sDoSetValue(0);
        }else{
            value = -1;
            QMessageBox box(QMessageBox::Question,tr("调取测试程序"),tr("无测试程序匹配，请手动选择！"));
            box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok,tr("确 定"));
            box.exec();
            sDoSetValue(0);
        }
    }else{
        QMessageBox box(QMessageBox::Question,tr("调取测试程序"),tr("条码与编码规则不匹配!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        value = -1;
        sDoSetValue(0);
    }

    db->removeDatabase("dbbarCode");
    db->close();
    delete db;
    return value;
}

void MainWindow::roleTrigger()
{
    if(tc.state == 3)
        return;

    if(tc.userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("参数设置"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    parameterRule *param;
    param = new parameterRule();
    param->setWindowFlags(param->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    param->setFixedSize(param->width(),param->height());  //固定大小，即禁止调整窗口大小
    param->show();
}

void MainWindow::seniorTrigger()
{
    if(tc.state == 3)
        return;

    if(tc.userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("参数设置"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    parameterSenior *param;
    param = new parameterSenior();
    param->setWindowFlags(param->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    param->setFixedSize(param->width(),param->height());  //固定大小，即禁止调整窗口大小
    param->show();
}

void MainWindow::rangeTrigger()
{
    if(tc.state == 3)
        return;

    if(tc.userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("参数设置"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    parameterRange *param;
    param = new parameterRange();
    param->setWindowFlags(param->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    param->setFixedSize(param->width(),param->height());  //固定大小，即禁止调整窗口大小
    param->show();
}

void MainWindow::fileTrigger()
{
    if(tc.state == 3)
        return;

    if(tc.userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("参数设置"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    parameterFile *param;
    param = new parameterFile();
    param->setWindowFlags(param->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    param->setFixedSize(param->width(),param->height());  //固定大小，即禁止调整窗口大小
    param->show();
}

void MainWindow::userTrigger()
{
    if(tc.state == 3)
        return;

    if(tc.userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("参数设置"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    parameterUser *param;
    param = new parameterUser();
    param->setWindowFlags(param->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    param->setFixedSize(param->width(),param->height());  //固定大小，即禁止调整窗口大小
    param->show();
}

void MainWindow::receiveData(QString data)
{
    openGroupName = data;
}

void MainWindow::chooseTrigger()
{
    if(tc.state == 3)
        return;

    chooseGroup *group;
    group = new chooseGroup();
    group->setWindowFlags(group->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    group->setFixedSize(group->width(),group->height());  //固定大小，即禁止调整窗口大小
    group->userLevel = tc.userLevel;
    group->readGroup(ui->label_20->text());
    connect(group, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    connect(group, SIGNAL(sendGroupName(int,QString)), this, SLOT(sendGroupDataByEdit(int,QString)));
    openType = 0;

    if(group->exec() == QDialog::Accepted)
    {
        editGroups *g;
        g = new editGroups();
        g->setWindowFlags(g->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
        g->setFixedSize(g->width(),g->height());  //固定大小，即禁止调整窗口大小
        g->setCommOffset(2);
        QDesktopWidget* desktop = QApplication::desktop();
        connect(g, SIGNAL(sendGroupFromEdit(int,QString)), this, SLOT(sendGroupDataByEdit(int,QString)));

        if(openType == 0)
        {
            g->readGroupFromFile(openGroupName);
            g->show();
            g->setItmeSize();
            g->singAndSlotConn();
            g->move((desktop->width() - g->width())/2, (desktop->height() - g->height())/2);
        }else{
            g->show();
            g->newGroup();
            g->singAndSlotConn();
            g->move((desktop->width() - g->width())/2, (desktop->height() - g->height())/2);
        }
    }

//    if(group->exec() == QDialog::Accepted)
//    {
//        readGroupFromFile(openGroupName);

//        if(groupData.groupNum == 0)
//            selfOffset = 1;
//        else
//            selfOffset = 0;

//        emit sendGroupData(groupData);
//        showGroup();
//    }
}

void MainWindow::selfCheck()
{
    if(tc.state == 3)
        return;

    selfcheck *group;
    group = new selfcheck();
    group->setWindowFlags(group->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    group->setFixedSize(group->width(),group->height());  //固定大小，即禁止调整窗口大小
    group->userLevel = tc.userLevel;
    group->readSelfCheckGroup();
    connect(group, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    openType = 0;
    if(group->exec() == QDialog::Accepted)
    {
        editGroups *g;
        g = new editGroups();
        g->setWindowFlags(g->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
        g->setFixedSize(g->width(),g->height());  //固定大小，即禁止调整窗口大小
        g->setCommOffset(1);
        QDesktopWidget* desktop = QApplication::desktop();
        connect(g, SIGNAL(sendGroupFromEdit(int,QString)), this, SLOT(sendGroupDataByEdit(int,QString)));

        if(openType == 0)
        {
            selfOffset = 1;
            readSelfCheckGroup(openGroupName);
            sendGroupData(groupOffset);
            delay(500);
            showSelfCheckGroup();
            delay(500);
            testStartFun();
        }else if(openType == 2){
            g->readGroupFromFile(openGroupName);
            g->show();
            g->setItmeSize();
            g->singAndSlotConn();
            g->move((desktop->width() - g->width())/2, (desktop->height() - g->height())/2);
        }else{
            g->show();
            g->newGroup();
            g->singAndSlotConn();
            g->move((desktop->width() - g->width())/2, (desktop->height() - g->height())/2);
        }
    }
}

void MainWindow::sendGroupDataByEdit(int gn,QString fileName)
{
    if(tc.state == 3)
        return;

    if(info->isVisible())
    {
        alarmTimer->stop();
        info->setVisible(false);
    }

    if(gn == 1)
    {
        selfOffset = 1;
        readSelfCheckGroup(fileName);
        sendGroupData(groupOffset);
        delay(600);
        showSelfCheckGroup();
    }else{        
        if(tc.machineModel == 4)
        {
            int ok = 0;
            ok = groupCheckPPL(fileName);
            if(ok == -1)
            {
                QMessageBox box(QMessageBox::Warning,tr("测试程序"),tr("所选测试程序与当前通讯协议不兼容，请重新编辑测试程序!"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }
        }
        readGroupFromFile(fileName);
        emit sendGroupData(groupData);
        showGroup();
    }
}

void MainWindow::groupTrigger()
{
    if(tc.state == 3)
        return;

    chooseGroup *group;
    group = new chooseGroup();
    group->setWindowFlags(group->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    group->setFixedSize(group->width(),group->height());  //固定大小，即禁止调整窗口大小
    group->userLevel = tc.userLevel;
    group->readGroup(ui->label_20->text());
    connect(group, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    openType = 0;
    if(group->exec() == QDialog::Accepted)
    {
        editGroups *g;
        g = new editGroups();
        g->setWindowFlags(g->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
        g->setFixedSize(g->width(),g->height());  //固定大小，即禁止调整窗口大小
        g->setCommOffset(2);
        QDesktopWidget* desktop = QApplication::desktop();
        connect(g, SIGNAL(sendGroupFromEdit(int,QString)), this, SLOT(sendGroupDataByEdit(int,QString)));

        if(openType == 0)
        {
            g->readGroupFromFile(openGroupName);
            g->show();
            g->setItmeSize();
            g->singAndSlotConn();
            g->move((desktop->width() - g->width())/2, (desktop->height() - g->height())/2);
        }else{
            g->show();
            g->newGroup();
            g->singAndSlotConn();
            g->move((desktop->width() - g->width())/2, (desktop->height() - g->height())/2);
        }
    }
}

void MainWindow::registerTrigger()
{
    registe *rg;
    rg = new registe();
    rg->setWindowFlags(rg->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    rg->setFixedSize(rg->width(),rg->height());  //固定大小，即禁止调整窗口大小
    rg->show();
}

void MainWindow::reportTrigger()
{
    if(tc.state == 3)
        return;

    queryAndReport *qp;
    qp = new queryAndReport();
    qp->setWindowFlags(qp->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    qp->setFixedSize(qp->width(),qp->height());  //固定大小，即禁止调整窗口大小
    qp->show();
    qp->iniQuery(tc.userName);
}

void MainWindow::configTrigger()
{
    if(tc.state == 3)
        return;

    if(tc.userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("参数设置"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    parameter *param;
    param = new parameter();
    connect(param, SIGNAL(sendCommand(int)),this,SLOT(openCloseSerial(int)));
    connect(param, SIGNAL(sendScanner(int)),this,SLOT(openCloseScanner(int)));
    connect(param, SIGNAL(sendIpRemote()),this,SLOT(setIpRemoteByPara()));
    param->readParameter(0);
    param->setWindowFlags(param->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);    //禁止最大和最小化
    param->setFixedSize(param->width(),param->height());  //固定大小，即禁止调整窗口大小
    param->show();
}

void MainWindow::setIpRemoteByPara()
{
    emit setIpRemoteFromparameter();
}

/*************************************
 * 函数名：resizeEvent
 * 说明：测试项缩放
*************************************/
void MainWindow::resizeEvent(QResizeEvent *size)
{    
    if(canDo == 1)
    {
        int h = ui->listView->size().height()/8;
        int w = ui->listView->size().width();
        for(int i=0;i<groupData.itemNum;i++)
        {
            standardItem_List[i]->setSizeHint(QSize(0,h));
            testItem_List[i]->setItemSize(w,h);
        }

        if(tc.machineModel == 6)
        {
            standardItem_List[groupData.itemNum]->setSizeHint(QSize(0,h));
            testItem_XSB->setItemSize(w,h);
        }
    }
}

/*************************************
 * 函数名：readRangeFromDB
 * 说明：
*************************************/
void MainWindow::readRangeFromDB(range_Item set[])
{    
    //连接数据库
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";      //数据库文件
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QODBC");
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db.setDatabaseName(dsn);    //设置连接字符串
    db.setUserName("");     //设置登陆数据库的用户名
    db.setPassword("");     //设置密码
    bool ok = db.open();

    if(!ok)
    {
        return;
    }

    QSqlQuery q;
    QString sql = "select TIName,ID,Active,vDefault,vMinDefault,vMin,vMaxDefault,vMax,vFormatDefault,"
                  "vFormat,vUnit,crpDefault,crpMinDefault,crpMin,crpMaxDefault,crpMax,crpFormatDefault,"
                  "crpFormat,crpUnit,timeDefault,timeMinDefault,timeMin,timeMaxDefault,timeMax,"
                  "timeFormatDefault,timeFormat,timeUnit,spareDefault,spareMinDefault,spareMin,"
                  "spareMaxDefault,spareMax,spareFormatDefault,spareFormat,spareUnit"
                  " from ItemRange order by ID";
    q.exec(sql);
    int i=0;
    while(q.next())
    {
        set[i].id = q.value(1).toInt();
        set[i].active = q.value(2).toInt();

        set[i].def[0] = q.value(3).toFloat();
        set[i].minDefault[0] = q.value(4).toFloat();
        set[i].min[0] = q.value(5).toFloat();
        set[i].maxDefault[0] = q.value(6).toFloat();
        set[i].max[0] = q.value(7).toFloat();
        set[i].precDefault[0] = q.value(8).toInt();
        set[i].prec[0] = q.value(9).toInt();
        set[i].unit[0] = q.value(10).toString();

        set[i].def[1] = q.value(11).toFloat();
        set[i].minDefault[1] = q.value(12).toFloat();
        set[i].min[1] = q.value(13).toFloat();
        set[i].maxDefault[1] = q.value(14).toFloat();
        set[i].max[1] = q.value(15).toFloat();
        set[i].precDefault[1] = q.value(16).toInt();
        set[i].prec[1] = q.value(17).toInt();
        set[i].unit[1] = q.value(18).toString();

        set[i].def[2] = q.value(19).toFloat();
        set[i].minDefault[2] = q.value(20).toFloat();
        set[i].min[2] = q.value(21).toFloat();
        set[i].maxDefault[2] = q.value(22).toFloat();
        set[i].max[2] = q.value(23).toFloat();
        set[i].precDefault[2] = q.value(24).toInt();
        set[i].prec[2] = q.value(25).toInt();
        set[i].unit[2] = q.value(26).toString();

        set[i].def[3] = q.value(27).toFloat();
        set[i].minDefault[3] = q.value(28).toFloat();
        set[i].min[3] = q.value(29).toFloat();
        set[i].maxDefault[3] = q.value(30).toFloat();
        set[i].max[3] = q.value(31).toFloat();
        set[i].precDefault[3] = q.value(32).toInt();
        set[i].prec[3] = q.value(33).toInt();
        set[i].unit[3] = q.value(34).toString();
        i++;
    }
    db.close();

    itemUnits[0]<<tr("电压")+QString("(%1)").arg(set[0].unit[0])<<tr("电流")+QString("(%1)").arg(set[0].unit[1])<<""<<tr("时间")+QString("(%1)").arg(set[0].unit[2]);
    itemUnits[1]<<tr("电压")+QString("(%1)").arg(set[1].unit[0])<<tr("电流")+QString("(%1)").arg(set[1].unit[1])<<""<<tr("时间")+QString("(%1)").arg(set[1].unit[2]);
    itemUnits[2]<<tr("电压")+QString("(%1)").arg(set[2].unit[0])<<tr("电阻")+QString("(%1)").arg(set[2].unit[1])<<""<<tr("时间")+QString("(%1)").arg(set[2].unit[2]);
    itemUnits[3]<<tr("电流")+QString("(%1)").arg(set[3].unit[3])<<tr("电阻")+QString("(%1)").arg(set[3].unit[1])<<""<<tr("时间")+QString("(%1)").arg(set[3].unit[2]);
    if(tc.phase == 1 || tc.phase == 2)
        itemUnits[4]<<tr("电压")+QString("(%1)").arg(set[4].unit[0])<<tr("电流")+QString("(%1)").arg(set[4].unit[1])<<tr("功率")+"(W)"<<tr("时间")+QString("(%1)").arg(set[4].unit[2]);
    else
        itemUnits[4]<<tr("电压")+QString("(%1)").arg(set[4].unit[0])<<tr("电流")+QString("(%1)").arg(set[4].unit[1])<<""<<tr("时间")+QString("(%1)").arg(set[4].unit[2]);
    itemUnits[5]<<tr("电压")+QString("(%1)").arg(set[5].unit[0])<<tr("电流")+QString("(%1)").arg(set[5].unit[3])<<tr("功率")+QString("(%1)").arg(set[5].unit[1])<<tr("时间")+QString("(%1)").arg(set[5].unit[2]);
    itemUnits[6]<<tr("电压")+QString("(%1)").arg(set[6].unit[0])<<tr("电流")+QString("(%1)").arg(set[6].unit[1])<<tr("功率")+QString("(%1)").arg(set[6].unit[3])<<tr("时间")+QString("(%1)").arg(set[6].unit[2]);
    itemUnits[7]<<tr("电压")+QString("(%1)").arg(set[7].unit[0])<<tr("电阻")+QString("(%1)").arg(set[7].unit[1])<<""<<tr("时间")+QString("(%1)").arg(set[7].unit[2]);
}

void MainWindow::openCloseScanner(int cmd)
{
    if(cmd == 0)
    {
        closeScanner();
    }else{
        openScanner();
    }
}

void MainWindow::openCloseSerial(int index)
{
    QByteArray tt;

    if(index == 2)
    {
        QString a = QString("%1%2%3%4%5%6%7%8%9%10%11%12%13%14%15%16")
                .arg(0).arg(0).arg(0).arg(0).arg(0).arg(0).arg(0).arg(0)
                .arg(tc.SP.leakageOffet).arg(tc.SP.GNDOffset).arg(tc.SP.ACOffset)
                .arg(tc.SP.DCOffset).arg(tc.SP.ground).arg(tc.SP.alarm).arg(tc.SP.plc).arg(0);
        tt = a.toLatin1();
        emit sendCommand(2,1,tt);

    }else{
        tt.append("ThreadTest");
        emit sendCommand(index,1,tt);
    }
}

void MainWindow::slotreply()
{
    QByteArray array = m_reply->readAll();
    QJsonParseError json_erron;
    QJsonDocument doc = QJsonDocument::fromJson(array,&json_erron);
    if(json_erron.error != QJsonParseError::NoError)
    {
        return;
    }

    if(doc.isObject())
    {
        QJsonObject obj = doc.object();

        if (obj.contains("success"))
        {
            QJsonValue value = obj.value("success");
            if (value.isBool())
            {
                bool v = value.toBool();
                qDebug()<<v;
            }
        }
    }
}

int MainWindow::barCodeCheck(QString sn)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest *request = new QNetworkRequest();
    QNetworkReply *m_reply = NULL;

    QByteArray postArray;
    QString value = QString("productCode=%1").arg(sn);
    postArray.append(value);

    request->setUrl(QUrl(tc.url_1));
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    m_reply = manager->post(*request,postArray);

    QEventLoop loop;
    connect(sysTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(m_reply, &QNetworkReply::readyRead, &loop, &QEventLoop::quit);
    sysTimer->start();
    loop.exec();

    if(sysTimer->isActive())
    {
        sysTimer->stop();
        if(m_reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "Error String : " << m_reply->errorString();
            loop.deleteLater();
            m_reply = NULL;
            manager->deleteLater();
            delete request;
            return -1;
        }else{
            QByteArray array = m_reply->readAll();
            QJsonParseError json_erron;
            QJsonDocument doc = QJsonDocument::fromJson(array,&json_erron);
            if(json_erron.error != QJsonParseError::NoError)
            {
                qDebug()<<json_erron.errorString();
                loop.deleteLater();
                m_reply->deleteLater();
                manager->deleteLater();
                delete request;
                return -1;
            }

            if(doc.isObject())
            {
                QJsonObject obj = doc.object();

                if(obj.contains("success"))
                {
                    QJsonValue jValue = obj.value("success");
                    if(jValue.isBool())
                    {
                        bool v = jValue.toBool();
                        if(v == false)
                        {
                            loop.deleteLater();
                            m_reply->deleteLater();
                            manager->deleteLater();
                            delete request;
                            return -1;
                        }else{
                            loop.deleteLater();
                            m_reply->deleteLater();
                            manager->deleteLater();
                            delete request;
                            return 0;
                        }
                    }else{
                        loop.deleteLater();
                        m_reply->deleteLater();
                        manager->deleteLater();
                        delete request;
                        return -1;
                    }
                }else{
                    loop.deleteLater();
                    m_reply->deleteLater();
                    manager->deleteLater();
                    delete request;
                    return -1;
                }
            }else{
                loop.deleteLater();
                m_reply->deleteLater();
                manager->deleteLater();
                delete request;
                return -1;
            }
        }
    }else{
        disconnect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        m_reply->abort();
        m_reply->deleteLater();
        return -1;
    }

    return 0;
}

/*******************************
 * 方法名：groupCheckPPL
 * 说明：检查测试程序是否符合PPL标准
*******************************/
int MainWindow::groupCheckPPL(QString name)
{
    QString fileName = QCoreApplication::applicationDirPath()+"/files/"+name+".txt";
    if(!QFile::exists(fileName))
        return -1;

    QSettings *configIniRead = new QSettings(fileName, QSettings::IniFormat);
    configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

    QStringList group;

    configIniRead->beginGroup("ItemList");
    group = configIniRead->childKeys();

    int ok = 0;
    int pn = 0;
    QString a = NULL;
    QString b = NULL;
    QStringList list;
    QList<int> pList;
    for(int i=1;i<=group.count();i++)
    {
        a = "item"+QString::number(i,10);
        b = configIniRead->value(a).toString();
        list = b.split(",");

        if(list.at(1).toInt() == 6)
        {
            pList<<i-1;
            ok++;
        }
    }

    if(ok == 0 || ok == group.count())
    {
        configIniRead->endGroup();
        delete configIniRead;
        return 1;
    }

    if(!pList.isEmpty())
    {
        if(pList.last() != group.count()-1)
        {
            configIniRead->endGroup();
            delete configIniRead;
            return -1;
        }
    }

    ok = 0;
    int la = pList.last();
    for(int i=pList.size()-1;i>=0;i--)
    {
        if(pList.at(i) != la)
            ok = -1;
        la--;
    }

    configIniRead->endGroup();
    delete configIniRead;
    return ok;
}

void MainWindow::mdbFileFun()
{
    QDate time = QDate::currentDate();
    QString fileName = time.toString("yyyy-MM-dd")+".mdb";
    QString sDbNm = QCoreApplication::applicationDirPath()+"/data/"+fileName;

    for(int i=0;i<5;i++)
    {
        if(!QFile::exists(sDbNm))
        {
            QFile::copy(QCoreApplication::applicationDirPath()+"/db/TestData.mdb",sDbNm);
            tc.iTotal = 0;
            tc.iPass = 0;
            tc.iNum[0] = 0;
            tc.iNum[1] = 0;
            tc.iNum[2] = 0;
            tc.iNum[3] = 0;
            tc.iNum[4] = 0;
            tc.iNum[5] = 0;
            tc.iNum[6] = 0;
            tc.iNum[7] = 0;
            tc.iNGNum[0] = 0;
            tc.iNGNum[1] = 0;
            tc.iNGNum[2] = 0;
            tc.iNGNum[3] = 0;
            tc.iNGNum[4] = 0;
            tc.iNGNum[5] = 0;
            tc.iNGNum[6] = 0;
            tc.iNGNum[7] = 0;
        }else{
            break;
        }
    }
}
