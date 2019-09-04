#include "parameterbase.h"
#include "ui_parameterbase.h"

parameterBase::parameterBase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameterBase)
{
    ui->setupUi(this);
    readParameter();
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitParameter()));
}

parameterBase::~parameterBase()
{
    delete ui;
}

void parameterBase::inicomm()
{
    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM";
    QSettings *settings = new QSettings(path, QSettings::NativeFormat);
    QStringList key = settings->allKeys();
    QStringList comlist;

    comlist.clear();
    ui->comboBox_2->clear();
    ui->comboBox_3->clear();
    ui->comboBox_9->clear();
    ui->comboBox_11->clear();

    for(int i=0; i<key.size(); i++)
    {
        comlist << getcomm(i, "value");
    }
    ui->comboBox_2->addItems(comlist);
    ui->comboBox_3->addItems(comlist);
    ui->comboBox_9->addItems(comlist);
    ui->comboBox_11->addItems(comlist);

    qDebug()<<key;
    qDebug()<<comlist;
    delete settings;
}

QString parameterBase::getcomm(int index,QString keyorvalue)
{
    QString commresult;

    if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != 0)
    {
        QString error="Cannot open regedit!";
        return error;
    }

    QString keymessage;
    QString message;
    QString valuemessage;

    indexnum = index;
    keysize = sizeof(keyname);
    valuesize = sizeof(keyvalue);

    if(RegEnumValue(hKey, indexnum, keyname, &keysize, 0, &type, (BYTE*)keyvalue, &valuesize) == 0)
    {
        for (int i=0; i<(int)keysize; i++)
        {
            message = keyname[i];
            keymessage.append(message);
        }

        for(int j=0; j<(int)valuesize; j++)
        {
            if (keyvalue[j] != 0x00) {
                valuemessage.append(keyvalue[j]);
            }
        }

        if(keyorvalue == "key")
        {
            commresult = keymessage;
        }

        if(keyorvalue == "value")
        {
            commresult=valuemessage;
        }
    }else{
        commresult = "nokey";
    }

    RegCloseKey(hKey);

    return commresult;
}

void parameterBase::readParameter()
{
    inicomm();

    if(tc.codeMode == 0)
    {
        ui->radioButton_3->setChecked(true);
        ui->radioButton_4->setChecked(false);
    }else{
        ui->radioButton_3->setChecked(false);
        ui->radioButton_4->setChecked(true);
    }

    ui->lineEdit_2->setText(tc.AC.prefix);
    ui->spinBox_2->setValue(tc.AC.code);
    ui->spinBox_4->setValue(tc.AC.length);
    ui->lineEdit_5->setText(tc.AC.suffix);

    if(tc.AC.down == 1)
        ui->checkBox_4->setCheckState(Qt::Checked);
    else
        ui->checkBox_4->setCheckState(Qt::Unchecked);

    if(tc.AC.keep == 1)
        ui->checkBox_9->setCheckState(Qt::Checked);
    else
        ui->checkBox_9->setCheckState(Qt::Unchecked);

    if(tc.BI.setByBarCode == 1)
        ui->checkBox_6->setCheckState(Qt::Checked);
    else
        ui->checkBox_6->setCheckState(Qt::Unchecked);

    if(tc.BI.startByBarCode == 1)
        ui->checkBox_8->setCheckState(Qt::Checked);
    else
        ui->checkBox_8->setCheckState(Qt::Unchecked);

    if(tc.STD.savePass == 1)
        ui->checkBox_5->setCheckState(Qt::Checked);
    else
        ui->checkBox_5->setCheckState(Qt::Unchecked);

    if(tc.STD.savaFail == 1)
        ui->checkBox_7->setCheckState(Qt::Checked);
    else
        ui->checkBox_7->setCheckState(Qt::Unchecked);

    if(tc.STD.upload == 1)
    {
        ui->checkBox_10->setCheckState(Qt::Checked);
        ui->comboBox_7->setVisible(true);
        ui->comboBox_7->setCurrentIndex(tc.STD.uploadType);
    }else{
        ui->checkBox_10->setCheckState(Qt::Unchecked);
        ui->comboBox_7->setVisible(false);
        ui->comboBox_7->setCurrentIndex(tc.STD.uploadType);
    }

    if(tc.PS.printFail == 1)
        ui->checkBox_12->setCheckState(Qt::Checked);
    else
        ui->checkBox_12->setCheckState(Qt::Unchecked);

    if(tc.PS.printPass == 1)
        ui->checkBox_11->setCheckState(Qt::Checked);
    else
        ui->checkBox_11->setCheckState(Qt::Unchecked);

    if(tc.PS.autoPrint == 1)
        ui->checkBox_13->setCheckState(Qt::Checked);
    else
        ui->checkBox_13->setCheckState(Qt::Unchecked);

    //串口
    ui->label_32->setVisible(false);
//    QList<QSerialPortInfo> portsList = QSerialPortInfo::availablePorts();
//    ui->comboBox_2->clear();
//    ui->comboBox_3->clear();
//    ui->comboBox_9->clear();
//    ui->comboBox_11->clear();
////    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
//    for(auto everyport:portsList)
//    {
//        int a = everyport.portName().mid(3,1).toInt();
//        QString n = everyport.portName();
////        qDebug()<<"port: "<<everyport.portName();
//        ui->comboBox_2->insertItem(a,n);
//        ui->comboBox_3->insertItem(a,n);
//        ui->comboBox_9->insertItem(a,n);
//        ui->comboBox_11->insertItem(a,n);
//    }
    ui->comboBox_2->setCurrentText(QString("COM%1").arg(tc.SPS.port));
    ui->comboBox_3->setCurrentText(QString("COM%1").arg(tc.SPS.sPort));
    ui->comboBox->setCurrentText(QString("%1").arg(tc.SPS.baudRate));
    ui->comboBox_4->setCurrentText(QString("%1").arg(tc.SPS.sBaudRate));
    ui->spinBox_5->setValue(tc.SPS.machineAddr);
    ui->comboBox_9->setCurrentText(QString("COM%1").arg(tc.SPS.pplPort));
    ui->comboBox_8->setCurrentText(QString("%1").arg(tc.SPS.pplBaudRate));
    ui->comboBox_11->setCurrentText(QString("COM%1").arg(tc.SPS.cPort));
    ui->comboBox_10->setCurrentText(QString("%1").arg(tc.SPS.cBaudRate));

    if(tc.SPS.sEnable == 1)
        ui->checkBox->setCheckState(Qt::Checked);
    else
        ui->checkBox->setCheckState(Qt::Unchecked);

    if(tc.portOpen == 1)
        ui->toolButton_7->setText(tr("关闭"));
    else
        ui->toolButton_7->setText(tr("打开"));

    if(tc.sPortOpen == 1)
        ui->toolButton_3->setText(tr("关闭"));
    else
        ui->toolButton_3->setText(tr("打开"));

    ui->lineEdit_3->setText(tc.DS.ip);
    ui->spinBox->setValue(tc.DS.port);
    ui->lineEdit_7->setText(tc.DS.dbName);
    ui->lineEdit_8->setText(tc.DS.machineNum);
    ui->lineEdit_9->setText(tc.DS.loginName);
    ui->lineEdit_10->setText(tc.DS.password);

    if(tc.txtEnable == 1)
    {
        ui->checkBox_2->setCheckState(Qt::Checked);
        ui->lineEdit_16->setReadOnly(false);
    }else{
        ui->checkBox_2->setCheckState(Qt::Unchecked);
        ui->lineEdit_16->setReadOnly(true);
    }

    ui->lineEdit_16->setText(tc.txtFile);
    ui->lineEdit_17->setText(tc.url_1);
    ui->lineEdit_18->setText(tc.url_2);
    ui->lineEdit_19->setText(tc.url_3);

    if(tc.tcpScanEnable == 1)
        ui->checkBox_14->setCheckState(Qt::Checked);
    else
        ui->checkBox_14->setCheckState(Qt::Unchecked);

    ui->lineEdit_13->setText(tc.tcpScanIP);
    ui->spinBox_3->setValue(tc.tcpScanPort);

    connect(ui->toolButton_4,SIGNAL(clicked(bool)),this,SLOT(txtFilePath()));
    connect(ui->toolButton_3,SIGNAL(clicked(bool)),this,SLOT(openCloseSPort()));
    connect(ui->toolButton_7,SIGNAL(clicked(bool)),this,SLOT(openClosePort()));
    connect(ui->toolButton_15,SIGNAL(clicked(bool)),this,SLOT(saveParameter()));
    connect(ui->spinBox_2,SIGNAL(valueChanged(QString)),this,SLOT(setLength(QString)));
    connect(ui->checkBox_10,SIGNAL(stateChanged(int)),this,SLOT(uploadSet()));
}

/*************************************
 * 函数名：     setLength
 * 说明：
*************************************/
void parameterBase::setLength(QString t)
{
    ui->spinBox_4->setValue(t.length());
}

void parameterBase::exitParameter()
{
    this->close();
    this->deleteLater();
}

/*************************************
 * 函数名：     saveParameter
 * 说明：
*************************************/
void parameterBase::saveParameter()
{
    QString port = ui->comboBox_2->currentText();
    QString sPort = ui->comboBox_3->currentText();
    QString cPort = ui->comboBox_11->currentText();

    if(port == sPort)
    {
        ui->label_32->setVisible(true);
        return;
    }
    ui->label_32->setVisible(false);

    QString tcpIp = ui->lineEdit_13->text();
    QRegExp rx("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
    if(!rx.exactMatch(tcpIp))
    {
        return;
    }

    if(ui->radioButton_3->isChecked())
        tc.codeMode = 0;
    else
        tc.codeMode = 1;

    tc.SPS.port = port.mid(3,port.length()-3).toInt();
    tc.SPS.sPort = sPort.mid(3,sPort.length()-3).toInt();

    tc.AC.prefix = ui->lineEdit_2->text();
    tc.AC.code = ui->spinBox_2->value();
    tc.AC.length = ui->spinBox_4->value();
    tc.AC.suffix = ui->lineEdit_5->text();

    if(ui->checkBox_4->isChecked())
        tc.AC.down = 1;
    else
        tc.AC.down = 0;

    if(ui->checkBox_9->isChecked())
        tc.AC.keep = 1;
    else
        tc.AC.keep = 0;

    if(ui->checkBox_6->isChecked())
        tc.BI.setByBarCode = 1;
    else
        tc.BI.setByBarCode = 0;

    if(ui->checkBox_8->isChecked())
        tc.BI.startByBarCode = 1;
    else
        tc.BI.startByBarCode = 0;

    if(ui->checkBox_5->isChecked())
        tc.STD.savePass = 1;
    else
        tc.STD.savePass = 0;

    if(ui->checkBox_7->isChecked())
        tc.STD.savaFail = 1;
    else
        tc.STD.savaFail = 0;

    if(ui->checkBox_10->isChecked())
        tc.STD.upload = 1;
    else
        tc.STD.upload = 0;

    tc.STD.uploadType = ui->comboBox_7->currentIndex();

    if(ui->checkBox_12->isChecked())
        tc.PS.printFail = 1;
    else
        tc.PS.printFail = 0;

    if(ui->checkBox_11->isChecked())
        tc.PS.printPass = 1;
    else
        tc.PS.printPass = 0;

    if(ui->checkBox_13->isChecked())
        tc.PS.autoPrint = 1;
    else
        tc.PS.autoPrint = 0;

    tc.SPS.baudRate = ui->comboBox->currentText().toInt();
    tc.SPS.sBaudRate = ui->comboBox_4->currentText().toInt();
    tc.SPS.cPort = cPort.mid(3,cPort.length()-3).toInt();
    tc.SPS.cBaudRate = ui->comboBox_10->currentText().toInt();
    tc.SPS.machineAddr = ui->spinBox_5->value();

    if(ui->checkBox->isChecked())
        tc.SPS.sEnable = 1;
    else
        tc.SPS.sEnable = 0;

    tc.SPS.pplPort = ui->comboBox_9->currentText().mid(3,port.length()-3).toInt();
    tc.SPS.pplBaudRate = ui->comboBox_8->currentText().toInt();

    tc.DS.ip = ui->lineEdit_3->text();
    tc.DS.port = ui->spinBox->value();
    tc.DS.dbName = ui->lineEdit_7->text();
    tc.DS.machineNum = ui->lineEdit_8->text();
    tc.DS.loginName = ui->lineEdit_9->text();
    tc.DS.password = ui->lineEdit_10->text();

    if(ui->checkBox_2->isChecked())
    {
        tc.txtEnable = 1;
        tc.txtFile = ui->lineEdit_16->text();
    }else{
        tc.txtEnable = 0;
    }
    tc.url_1 = ui->lineEdit_17->text();
    tc.url_2 = ui->lineEdit_18->text();
    tc.url_3 = ui->lineEdit_19->text();

    if(ui->checkBox_14->isChecked())
    {
        tc.tcpScanEnable = 1;
    }else{
        tc.tcpScanEnable = 0;
    }

    tc.tcpScanIP = ui->lineEdit_13->text();
    tc.tcpScanPort = ui->spinBox_3->value();
}

void parameterBase::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/*************************************
 * 函数名：     openClosePort
 * 说明：
*************************************/
void parameterBase::openClosePort()
{
    QString port = ui->comboBox_2->currentText();
    QString sPort = ui->comboBox_3->currentText();

    if(port == sPort)
    {
        ui->label_32->setVisible(true);
        return;
    }
    ui->label_32->setVisible(false);
    if(ui->toolButton_7->text() == tr("关闭"))
    {
        ui->toolButton_7->setText(tr("打开"));
        emit sendCommand(1);
    }else{
        ui->toolButton_7->setText(tr("关闭"));
        emit sendCommand(0);
    }
}

/*************************************
 * 函数名：     openCloseSPort
 * 说明：
*************************************/
void parameterBase::openCloseSPort()
{
    QString port = ui->comboBox_2->currentText();
    QString sPort = ui->comboBox_3->currentText();

    if(port == sPort)
    {
        ui->label_32->setVisible(true);
        return;
    }
    ui->label_32->setVisible(false);

    if(ui->toolButton_3->text() == tr("关闭"))
    {
        emit sendScanner(0);
        delay(200);
        if(tc.sPortOpen == 0)
        {
            ui->toolButton_3->setText(tr("打开"));
        }
    }else{
        emit sendScanner(1);
        delay(200);
        if(tc.sPortOpen == 1)
        {
            ui->toolButton_3->setText(tr("关闭"));
        }
    }
}

void parameterBase::txtFilePath()
{
    QString txt_path = QFileDialog::getExistingDirectory(this,tr("TXT文件目录"),".");
    ui->lineEdit_16->setText(txt_path);
}

void parameterBase::uploadSet()
{
    if(ui->checkBox_10->isChecked())
        ui->comboBox_7->setVisible(true);
    else
        ui->comboBox_7->setVisible(false);
}
