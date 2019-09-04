#include "loginDialog.h"
#include "loginsetinfo.h"
#include "ui_loginDialog.h"
#include "ui_test_main.h"
#include <QSettings>
#include <QDialog>
#include <QTextCodec>
#include <QTime>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlIndex>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquerymodel.h>
#include "test_main.h"

loginDlg::loginDlg(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::loginDlg)
{
    ui->setupUi(this);

    readIniFile();

    if(tc.language == 1)
    {
        this->setWindowTitle(tr("login"));
        ui->label->setText(tr("Username"));
        ui->label_2->setText(tr("Password"));
        ui->loginOK->setText(tr("Login"));
        ui->toolButton_2->setText(tr("Exit"));
    }

    readOperater();
    connect(ui->comboBox_2,SIGNAL(currentIndexChanged(int)),this,SLOT(pwSetFocus()));
    connect(ui->lineEdit_2,SIGNAL(returnPressed()),this,SLOT(on_loginOK_clicked()));
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),qApp,SLOT(quit()));
}

loginDlg::~loginDlg()
{
    delete ui;
}

bool loginDlg::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type()==QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        if(obj == ui->comboBox_2->view())
        {
            if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                pwSetFocus();
            }

            return true;
        }else if(obj == ui->comboBox_2){
            if(keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                pwSetFocus();
            }
            return true;
        }else
            return QObject::eventFilter(obj,e);
    }
}

void loginDlg::pwSetFocus()
{
    ui->lineEdit_2->setText("");
    ui->lineEdit_2->setFocus();
}

void loginDlg::readOperater()
{
    //连接数据库
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
    if(!QFile::exists(sDbNm))
    {
        QMessageBox::warning(0,"登录","数据库文件不存在，请检查软件是否已正确安装!",QMessageBox::Yes);
        return;
    }

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
    QString sql = "select User from UserList";
    q.exec(sql);

    int num = 0;
    ui->comboBox_2->clear();
    while(q.next())
    {
        ui->comboBox_2->insertItem(num++,q.value(0).toString());
    }
    q.clear();
    db.close();
    ui->comboBox_2->setCurrentIndex(-1);
    ui->comboBox_2->setCurrentText(tc.userName);    
}

int loginDlg::readIniFile()
{
    QString dirPath = QCoreApplication::applicationDirPath()+"/setInfo.ini";
    QFile file(dirPath);
    int i = 0;
    if(file.exists())
    {
        QSettings *configIniRead = new QSettings(dirPath, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

        tc.userName = configIniRead->value("main/userName").toString();
        tc.language = configIniRead->value("main/language").toInt();
        tc.startMode = configIniRead->value("main/startMode").toInt();

        tc.SPS.machineAddr = configIniRead->value("set/addr").toInt();
        tc.SPS.port = configIniRead->value("set/port").toInt();
        tc.SPS.baudRate = configIniRead->value("set/baudRate").toInt();
        tc.SPS.sPort = configIniRead->value("set/sPort").toInt();
        tc.SPS.sBaudRate = configIniRead->value("set/sBaudRate").toInt();
        tc.SPS.sEnable = configIniRead->value("set/sEnable").toInt();
        tc.lastGroup = configIniRead->value("set/lastGroup").toString();
        tc.total = configIniRead->value("set/testNum").toFloat();
        tc.passNum = configIniRead->value("set/passNum").toFloat();
        tc.codeMode = configIniRead->value("set/codeMode").toInt();
        tc.auxiliary1 = configIniRead->value("set/auxiliary1").toString();
        tc.auxiliary2 = configIniRead->value("set/auxiliary2").toString();
        tc.auxiliary3 = configIniRead->value("set/auxiliary3").toString();

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
        tc.AC.length = configIniRead->value("set/length").toInt();
        tc.AC.down = configIniRead->value("set/autoDown").toInt();
        tc.AC.keep = configIniRead->value("set/autoKeep").toInt();

        tc.BI.setByBarCode = configIniRead->value("set/setByBarCode").toInt();
        tc.BI.startByBarCode = configIniRead->value("set/startByBarcode").toInt();

        tc.STD.saveAll = configIniRead->value("set/saveAll").toInt();
        tc.STD.savePass = configIniRead->value("set/savePass").toInt();
        tc.STD.savaFail = configIniRead->value("set/saveFail").toInt();
        tc.STD.doNotSave = configIniRead->value("set/doNotSave").toInt();
        tc.STD.upload = configIniRead->value("set/upload").toInt();

        tc.DS.ip = configIniRead->value("set/dbIp").toString();
        tc.DS.dbName = configIniRead->value("set/dbName").toString();
        tc.DS.loginName = configIniRead->value("set/loginName").toString();
        tc.DS.password = configIniRead->value("set/password").toString();
        tc.DS.machineNum = configIniRead->value("set/machineNum").toString();
        tc.DS.port = configIniRead->value("set/dbPort").toInt();

        tc.PS.autoPrint = configIniRead->value("set/autoPrint").toInt();
        tc.PS.printFail = configIniRead->value("set/printFail").toInt();
        tc.PS.printPass = configIniRead->value("set/printPass").toInt();

        i = tc.language;
        delete configIniRead;
    }

    if(tc.language != 2)
        ui->comboBox->hide();
    else
        ui->comboBox->show();
    return i;
}

void loginDlg::writeIniFile()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QSettings *configIniWrite = new QSettings(QCoreApplication::applicationDirPath()+"/setInfo.ini",
                                              QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("GBK"));

    configIniWrite->setValue("/main/userName",ui->comboBox_2->currentText());
    configIniWrite->setValue("/main/loginDate",current_date_time.toString("yyyy-MM-dd hh:mm:ss"));
    delete configIniWrite;
}

/************************************
 *函数名：checkLogin
 *说明：
************************************/
void loginDlg::checkLogin()
{
    if(ui->comboBox_2->currentText().length() == 0)
        return;

    QString name = ui->comboBox_2->currentText();
    QString pwd = ui->lineEdit_2->text();

    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";      //数据库文件
    if(!QFile::exists(sDbNm))
    {
        QMessageBox::warning(0,"登录","数据库文件不存在，请检查软件是否已正确安装!",QMessageBox::Yes);
        return;
    }

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
    QSqlError error;

    QString sql = "select count(*) from UserList where User='"+name+"';";
    q.exec(sql);
    q.next();
    int num = q.value(0).toInt();
    q.clear();

    if(num == 0)
    {
        db.close();
        QMessageBox box(QMessageBox::Warning,"登录","无此用户名!");
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,"确 定");
        box.exec();
        return;
    }

    sql = "select Password,userLevel from UserList where User='"+name+"'";
    ok = q.exec(sql);
    if(!ok)
    {
        error = db.lastError();
        qDebug()<<error.text();
    }
    q.next();
    QString userPWD = q.value(0).toString();
    int level = q.value(1).toInt();
    db.close();

    if(pwd == userPWD)
    {
        tc.userName = name;
        tc.userLevel = level;
        this->writeIniFile();
        this->accept();

    }else{
        QMessageBox::warning(0,tr("登录"),tr("密码错误!"),QMessageBox::Yes);
        ui->lineEdit_2->clear();
    }
}

void loginDlg::on_loginOK_clicked()
{
    checkLogin();
}

void loginDlg::keyPressEvent(QKeyEvent *event)
{
    if((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && ui->comboBox_2->hasFocus())
        pwSetFocus();
}
