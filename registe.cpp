#include "registe.h"
#include "ui_registe.h"

registe::registe(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registe)
{
    ui->setupUi(this);
    getCpuID();
    registeInfo();
    connect(ui->toolButton,SIGNAL(clicked(bool)),this,SLOT(exitReg()));
}

registe::~registe()
{
    delete ui;
}

void registe::exitReg()
{
    this->close();
    this->deleteLater();
}

void registe::GetCPUID2()
{
    QStringList mac_list;
    QString strMac;
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    for(int i=0; i<ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if(iface.flags().testFlag(QNetworkInterface::IsUp) && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
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
        ui->textBrowser->setText(str.replace(":",""));
    }
}

QString registe::getHDLogicalID()
{   
    DWORD VolumeSerialNumber;
    GetVolumeInformation(L"C:\\",NULL,0,&VolumeSerialNumber,NULL,NULL,NULL,0);
    return QString::number(VolumeSerialNumber,16).toUpper();
}

void registe::getCpuID()
{
    unsigned long s1,s2,s3,s4;
    char string[128];
    char szCpuId[1024];
    char p1[128], p2[128];
    unsigned int eax = 0;
    unsigned int ebx,ecx,edx;

    QString strCPUID;
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

    ui->textBrowser->setText(cupInfo);
}

void registe::registeInfo()
{    
    if(tc.REG.regState <= 0)
    {
        ui->lineEdit->setText(tr("未授权"));
        ui->lineEdit_2->setText(tc.REG.keyID);
        ui->lineEdit_3->setText(tc.REG.expiry);
        ui->lineEdit_4->setText(tc.REG.userName);
    }else if(tc.REG.regState == 1){
        ui->lineEdit->setText(tr("已许可"));
        ui->lineEdit_2->setText(tc.REG.keyID);
        ui->lineEdit_3->setText(tr("永不过期"));
        ui->lineEdit_4->setText(tc.REG.userName);
    }else if(tc.REG.regState == 2){
        ui->lineEdit->setText(tr("已许可"));
        ui->lineEdit_2->setText(tc.REG.keyID);
        ui->lineEdit_3->setText(tc.REG.expiry);
        ui->lineEdit_4->setText(tc.REG.userName);
    }    
}

void registe::GetLogicalDrives()
{
    QList<QStorageInfo> list = QStorageInfo::mountedVolumes();
    int vNum = list.size();
    QStringList pf;
    qDebug() << "Volume Num: " << vNum;
    for(QStorageInfo& si : list)
    {
        qDebug() << "Name: " << si.name();
        qDebug() << "Block Size: " << si.blockSize();
        qDebug() << "Bytes Avaliable: " << si.bytesAvailable();
        qDebug() << "Bytes Free: " << si.bytesFree();
        qDebug() << "Bytes Total: " << si.bytesTotal();
        pf<<si.displayName();
        qDebug() << "Display Name: " << si.displayName();
        qDebug() << "File System Type: " << si.fileSystemType();
        qDebug() << "Is ReadOnly: " << si.isReadOnly();
        qDebug() << "Is Root: " << si.isRoot();
    }
    qDebug() << pf;
    qDebug() << QStorageInfo::root().device();

    UINT uiResult = ::GetDriveType(TEXT("G:\\"));
    switch(uiResult)
    {
    case DRIVE_CDROM:
        qDebug() << "It's a CDROM Drive ";
        break;
    case DRIVE_FIXED:
        qDebug() << "It's a Fixed Drive!";
        break;
    case DRIVE_NO_ROOT_DIR:
        qDebug() << "It is not a Root Dir!";
        break;
    case DRIVE_RAMDISK:
        qDebug() << "It's a Ram Disk!";
        break;
    case DRIVE_REMOTE:
        qDebug() << "It's Remote dir!";
        break;
    case DRIVE_REMOVABLE:
        qDebug() << "It's a removable disk!";
        break;
    case DRIVE_UNKNOWN:
        qDebug() << "It is a Unknown dir!";
        break;
    }
}

void registe::webServiceTest()
{
    QtSoapHttpTransport  *m_pHttp;
    m_pHttp = new QtSoapHttpTransport(this);
    connect(m_pHttp, SIGNAL(responseReady(const QtSoapMessage &)), this, SLOT(onResponse(const QtSoapMessage &)));

    QtSoapMessage message;

    message.setMethod("qqCheckOnline", "http://WebXml.com.cn/");

    m_pHttp->setAction("http://WebXml.com.cn/qqCheckOnline");

    m_pHttp->setHost("www.webxml.com.cn");

    QString strQQ = "6666";
    message.addMethodArgument("qqCode", "", strQQ);
    QString strXML = message.toXmlString();

    m_pHttp->submitRequest(message, "/webservices/qqOnlineWebService.asmx");
}

void registe::onResponse(const QtSoapMessage &response)
{
    QString strXML = response.toXmlString();
    QDomDocument doc;
    doc.setContent(strXML);

    QDomNodeList nodeList = doc.elementsByTagName("qqCheckOnlineResult");
    if(!nodeList.isEmpty())
    {
        QDomNode node = nodeList.at(0);
        QString strResult = node.toElement().text();
        QString strState("N/A");
        if(QString::compare(strResult, "Y") == 0)
        {
            strState = QString::fromLocal8Bit("在线");
        }
        else if(QString::compare(strResult, "N") == 0)
        {
            strState = QString::fromLocal8Bit("离线");
        }
        else if(QString::compare(strResult, "E") == 0)
        {
            strState = QString::fromLocal8Bit("QQ号码错误");
        }
        else if(QString::compare(strResult, "A") == 0)
        {
            strState = QString::fromLocal8Bit("商业用户验证失败");
        }
        else if(QString::compare(strResult, "V") == 0)
        {
            strState = QString::fromLocal8Bit("免费用户超过数量");
        }
        qDebug()<<strState;
    }
}

void registe::binaryFileTest()
{   
    QFile file("E:\\file.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    out << (quint32)0xA0B0C0D0;
    out << (qint32)123;

    out.setVersion(QDataStream::Qt_5_7);

    out << "2017-09-30";
    file.close();
}
