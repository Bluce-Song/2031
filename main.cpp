#include "login.h"
#include "test_main.h"
#include "loginDialog.h"
#include "loginsetinfo.h"
#include "ui_test_main.h"
#include "ui_loginDialog.h"
#include <QApplication>
#include <QMainWindow>
#include <QDebug>
#include <qapplication>
#include <QSharedMemory>
#include <QTranslator>
QTranslator qtrans;

void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:");
        break;

    case QtWarningMsg:
        text = QString("Warning:");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:");
    }

    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(text).arg(context_info).arg(msg).arg(current_date);

    QFile file("log.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

/******************************
 * 方法名：readIniFile
 * 说明：
******************************/
void readIniFile()
{
    QString dirPath = QCoreApplication::applicationDirPath()+"/setInfo.ini";
    QFile file(dirPath);

    if(file.exists())
    {
        QSettings *configIniRead = new QSettings(dirPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

        tc.userName = configIniRead->value("main/userName").toString();
        tc.language = configIniRead->value("main/language").toInt();
        tc.startMode = configIniRead->value("main/startMode").toInt();
        tc.machineModel = configIniRead->value("main/machineModel").toInt();

        tc.SPS.machineAddr = configIniRead->value("set/addr").toInt();
        tc.SPS.port = configIniRead->value("set/port").toInt();
        tc.SPS.baudRate = configIniRead->value("set/baudRate").toInt();
        tc.SPS.sPort = configIniRead->value("set/sPort").toInt();
        tc.SPS.sBaudRate = configIniRead->value("set/sBaudRate").toInt();
        tc.SPS.sEnable = configIniRead->value("set/sEnable").toInt();

        tc.SPS.pplPort = configIniRead->value("set/pplPort").toInt();
        tc.SPS.pplBaudRate = configIniRead->value("set/pplBaudRate").toInt();
        tc.SPS.cPort = configIniRead->value("set/cPort").toInt();
        tc.SPS.cBaudRate = configIniRead->value("set/cBaudRate").toInt();

        tc.SPS.psEnable = configIniRead->value("set/psEnable").toInt();
        tc.SPS.psPort1 = configIniRead->value("set/psPort1").toInt();
        tc.SPS.psPort2 = configIniRead->value("set/psPort2").toInt();
        tc.SPS.psBaudRate1 = configIniRead->value("set/psBaudRate1").toInt();
        tc.SPS.psBaudRate2 = configIniRead->value("set/psBaudRate2").toInt();
        tc.SPS.psCoefficient1 = configIniRead->value("set/psCoefficient1").toFloat();
        tc.SPS.psCoefficient2 = configIniRead->value("set/psCoefficient2").toFloat();
        tc.SPS.psCoefficient3 = configIniRead->value("set/psCoefficient3").toFloat();
        tc.SPS.psCoefficient4 = configIniRead->value("set/psCoefficient4").toFloat();
        tc.SPS.psMin = configIniRead->value("set/psMin").toFloat();
        tc.SPS.psMax = configIniRead->value("set/psMax").toFloat();

        tc.lastGroup = configIniRead->value("set/lastGroup").toString();
        tc.total = configIniRead->value("set/testNum").toInt();
        tc.passNum = configIniRead->value("set/passNum").toInt();
        tc.codeMode = configIniRead->value("set/codeMode").toInt();
        tc.phase = configIniRead->value("set/phase").toInt();
        tc.md = configIniRead->value("set/md").toInt();
        tc.itemNumMax = configIniRead->value("set/itemNumMax").toInt();

        tc.SP.leakageOffet = configIniRead->value("set/leakageOffet").toInt();
        tc.SP.GNDOffset = configIniRead->value("set/gndOffset").toInt();
        tc.SP.ACOffset = configIniRead->value("set/acOffset").toInt();
        tc.SP.DCOffset = configIniRead->value("set/dcOffset").toInt();
        tc.SP.ground = configIniRead->value("set/ground").toInt();
        tc.SP.alarm = configIniRead->value("set/alarm").toInt();
        tc.SP.plc = configIniRead->value("set/plc").toInt();

        tc.AC.prefix = configIniRead->value("set/prefix").toString();
        tc.AC.suffix = configIniRead->value("set/suffix").toString();
        tc.AC.code = configIniRead->value("set/code").toInt();
        tc.AC.lastCode = configIniRead->value("set/lastCode").toInt();
        tc.AC.length = configIniRead->value("set/length").toInt();
        tc.AC.down = configIniRead->value("set/autoDown").toInt();
        tc.AC.keep = configIniRead->value("set/autoKeep").toInt();

        tc.BI.setByBarCode = configIniRead->value("set/setByBarCode").toInt();
        tc.BI.startByBarCode = configIniRead->value("set/startByBarcode").toInt();
        tc.BI.lengthCheck = configIniRead->value("set/lengthCheck").toInt();
        tc.BI.barCodeLength = configIniRead->value("set/barCodeLength").toInt();

        tc.STD.saveAll = configIniRead->value("set/saveAll").toInt();
        tc.STD.savePass = configIniRead->value("set/savePass").toInt();
        tc.STD.savaFail = configIniRead->value("set/saveFail").toInt();
        tc.STD.doNotSave = configIniRead->value("set/doNotSave").toInt();
        tc.STD.upload = configIniRead->value("set/upload").toInt();
        tc.STD.uploadType = configIniRead->value("set/uploadType").toInt();
        tc.STD.repeat = configIniRead->value("set/repeat").toInt();
        tc.STD.repeatType = configIniRead->value("set/repeatType").toInt();

        tc.DS.ip = configIniRead->value("set/dbIp").toString();
        tc.DS.dbName = configIniRead->value("set/dbName").toString();
        tc.DS.loginName = configIniRead->value("set/loginName").toString();
        tc.DS.password = configIniRead->value("set/password").toString();
        tc.DS.machineNum = configIniRead->value("set/machineNum").toString();
        tc.DS.port = configIniRead->value("set/dbPort").toInt();

        tc.PS.autoPrint = configIniRead->value("set/autoPrint").toInt();
        tc.PS.PrintType = configIniRead->value("set/PrintType","0").toInt();
        tc.PS.printFail = configIniRead->value("set/printFail").toInt();
        tc.PS.printPass = configIniRead->value("set/printPass").toInt();
        tc.PS.PrintPort = configIniRead->value("set/PrintPort","6").toInt();
        tc.PS.PrintBaudRate = configIniRead->value("set/PrintBaudRate","9600").toInt();

        tc.iDate = configIniRead->value("set/iDate").toString();
        QDateTime current_date_time = QDateTime::currentDateTime();
        if(tc.iDate == current_date_time.toString("yyyy-MM-dd"))
        {
            tc.iTotal = configIniRead->value("set/iTotal").toInt();
            tc.iPass = configIniRead->value("set/iPass").toInt();

            QString tmp = configIniRead->value("set/iNum").toString();
            QStringList cdList = tmp.split(",");

            for(int i=0;i<cdList.count();i++)
            {
                QString s = cdList.at(i);
                tc.iNum[i] = s.toInt();
            }

            tmp = configIniRead->value("set/iNGNum").toString();
            cdList.clear();
            cdList = tmp.split(",");
            for(int i=0;i<cdList.count();i++)
            {
                QString s = cdList.at(i);
                tc.iNGNum[i] = s.toInt();
            }
        }else{
            tc.iTotal = 0;
            tc.iPass = 0;
            for(int i=0;i<8;i++)
            {
                tc.iNum[i] = 0;
                tc.iNGNum[i] = 0;
            }
        }

        tc.auxiliary1 = configIniRead->value("set/auxiliary1").toString();
        tc.auxiliary2 = configIniRead->value("set/auxiliary2").toString();
        tc.auxiliary3 = configIniRead->value("set/auxiliary3").toString();

        tc.txtEnable = configIniRead->value("set/txtEnable").toInt();
        tc.txtFile = configIniRead->value("set/txtFile").toString();
        tc.url_1 = configIniRead->value("set/url1").toString();
        tc.url_2 = configIniRead->value("set/url2").toString();
        tc.url_3 = configIniRead->value("set/url3").toString();

        tc.tcpScanEnable = configIniRead->value("set/tcpScanEnable").toInt();
        tc.tcpScanPort = configIniRead->value("set/tcpScanPort").toInt();
        tc.tcpScanIP = configIniRead->value("set/tcpScanIP").toString();

        tc.ACW_I_K = configIniRead->value("set/ACW_I_K","1").toString();

        //读入完成后删除指针
        delete configIniRead;
    }
}

int main(int argc, char *argv[])
{
    QSharedMemory shared("okingApp");
    if(shared.attach())
    {
        return 0;
    }
    shared.create(1);

    QApplication a(argc, argv);
    readIniFile();
    if(tc.language == 1)
    {
        QString name = QCoreApplication::applicationDirPath()+"/user_zh.qm";
        qtrans.load(name);
        a.installTranslator(&qtrans);
        qDebug()<<"123";
    }

    //注册MessageHandler
//    qInstallMessageHandler(outputMessage);

    loginDlg login;    //登陆界面    

    login.setWindowFlags(login.windowFlags() &~ Qt::WindowMinMaxButtonsHint);//禁止最大和最小化
    login.setFixedSize(login.width(),login.height());  //固定大小，即禁止调整窗口大小
    if(login.exec() == QDialog::Accepted)
    {      
        MainWindow w; //主界面
//        w.showFullScreen();     //全屏，遮盖任务栏
        w.showMaximized();     //最大化，不遮盖任务栏
        w.showGroup();
        w.canDo = 1;
//        a.installTranslator(w.qtrans);
        return a.exec();    //进入事件循环
    }else{
        return 0;
    }
//    return a.exec();
}
