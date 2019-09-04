#include "login.h"
#include "loginsetinfo.h"
#include "ui_login.h"
#include "ui_test_main.h"
#include <QSettings>
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

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),qApp,SLOT(quit()));
}

login::~login()
{
    delete ui;
}

void login::readIniFile()
{
    //读取初始化文件
    QSettings *configIniRead = new QSettings("setInfo.ini", QSettings::IniFormat);
    QString ipResult = configIniRead->value("main/userName").toString();
    QString portResult = configIniRead->value("set/port").toString();

    tc.userName = ipResult;
    tc.SPS.port = portResult.toInt();
    ui->lineEdit->setText(tc.userName);
    ui->lineEdit_2->setText(portResult);

    //读入入完成后删除指针
    delete configIniRead;
}

void login::writeIniFile()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    //读取初始化文件
    QSettings *configIniWrite = new QSettings("setInfo.ini", QSettings::IniFormat);

    //向ini文件中写入内容,setValue函数的两个参数是键值对
    //向ini文件的第一个节写入内容,ip节下的第一个参数
    configIniWrite->setValue("/main/loginDate",current_date_time.toString("yyyy-MM-dd hh:mm:ss"));
    //向ini文件的第一个节写入内容,ip节下的第二个参数
  //  configIniWrite->setValue("ip/second", "127.0.0.1");
    //向ini文件的第二个节写入内容,port节下的第一个参数
  //  configIniWrite->setValue("port/open", "2222");
    //写入完成后删除指针
    delete configIniWrite;
}

/*************************************/
/*函数名：checkLogin              *****/
/*说明：检查用户名、密码是否匹配      ****/
/************************************/
void login::checkLogin()
{
    QString name = ui->lineEdit->text();
    QString pwd = ui->lineEdit_2->text();

    QDir dir;
    QString sDbNm = dir.currentPath();      //数据库文件
    sDbNm += "/debug/DB/AIP9643U_UserManage.mdb";    

    //连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");   //设置数据库驱动
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db.setDatabaseName(dsn);    //设置连接字符串
    db.setUserName("");     //设置登陆数据库的用户名
    db.setPassword("");     //设置密码
    bool ok = db.open();

    if (!ok)
    {
        QMessageBox messageBox;
        messageBox.setText("Database error");
        messageBox.exec();
        return;
    }

//    QSqlQuery mquery = QSqlQuery::QSqlQuery(db);
//    bool isok=mquery.exec("select * from UserList;");

    QSqlQuery q;
    QString sql = "select Password from UserList where User='"+name+"'";
    q.exec(sql);
//    QSqlRecord rec = q.record();
//    int fieldCount = rec.count();
    q.next();
    QString userPWD = q.value(0).toString();
    db.close();

    if(pwd == userPWD)
    {
        this->close();

    }else{
        QMessageBox::warning(0,"登录","密码错误!",QMessageBox::Yes);
//        ui->lineEdit->clear();  //清空姓名nameLine
        ui->lineEdit_2->clear();  //清空密码passwardLine
//        ui->lineEdit->setFocus();  //将鼠标重新定位到nameLine
    }
}

void login::on_loginOK_clicked()
{
    checkLogin();
//    QDir dir;
//    QString sDbNm = dir.currentPath();
//    sDbNm += "/DB/AIP9643U_UserManage.mdb";
//    QMessageBox::warning(0,"PATH",sDbNm,QMessageBox::Yes);
}
