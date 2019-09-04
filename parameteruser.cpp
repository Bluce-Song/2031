#include "parameteruser.h"
#include "ui_parameteruser.h"

parameterUser::parameterUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameterUser)
{
    ui->setupUi(this);
    readParameter();
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitUser()));
}

parameterUser::~parameterUser()
{
    delete ui;
}

void parameterUser::exitUser()
{
    this->close();
    this->deleteLater();
}

void parameterUser::readParameter()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);
    disconnect(ui->tableWidget_4,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(readUserInfo(QTableWidgetItem*)));
    disconnect(ui->toolButton_5,SIGNAL(clicked(bool)),this,SLOT(addUser()));
    disconnect(ui->toolButton_6,SIGNAL(clicked(bool)),this,SLOT(deleteUser()));
    disconnect(ui->toolButton_8,SIGNAL(clicked(bool)),this,SLOT(saveUser()));
    disconnect(ui->toolButton_9,SIGNAL(clicked(bool)),this,SLOT(setUserPassword()));
    disconnect(ui->lineEdit_11,SIGNAL(returnPressed()),this,SLOT(confirmPassword()));
    disconnect(ui->lineEdit_12,SIGNAL(returnPressed()),this,SLOT(savePassword()));
    disconnect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(setUserPassword()));

    newUser = 0;
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit->setText("");
    ui->comboBox_5->setCurrentIndex(0);
    ui->tableWidget_4->setRowCount(0);
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
    QString sql = "select User,userLevel from UserList order by User";
    q.exec(sql);

    int row = 0;
    while(q.next())
    {
        ui->tableWidget_4->insertRow(row);

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFont(font);
        item->setTextAlignment(Qt::AlignCenter);

        item->setText(q.value(0).toString());
        ui->tableWidget_4->setItem(row,0,item);

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setFont(font);
        item1->setTextAlignment(Qt::AlignCenter);

        if(q.value(1).toInt() == 0)
            item1->setText(tr("操作员"));
        else
            item1->setText(tr("管理员"));

        ui->tableWidget_4->setItem(row,1,item1);
        row++;
    }
    db.close();
    connect(ui->tableWidget_4,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(readUserInfo(QTableWidgetItem*)));
    connect(ui->toolButton_5,SIGNAL(clicked(bool)),this,SLOT(addUser()));
    connect(ui->toolButton_6,SIGNAL(clicked(bool)),this,SLOT(deleteUser()));
    connect(ui->toolButton_8,SIGNAL(clicked(bool)),this,SLOT(saveUser()));
    connect(ui->toolButton_9,SIGNAL(clicked(bool)),this,SLOT(setUserPassword()));
    connect(ui->lineEdit_11,SIGNAL(returnPressed()),this,SLOT(confirmPassword()));
    connect(ui->lineEdit_12,SIGNAL(returnPressed()),this,SLOT(savePassword()));
    connect(ui->lineEdit,SIGNAL(returnPressed()),this,SLOT(setUserPassword()));
}

/*************************************
 * 函数名：     readUserInfo
 * 说明：
*************************************/
void parameterUser::readUserInfo(QTableWidgetItem* item)
{
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit->setText(ui->tableWidget_4->item(item->row(),0)->text());
    ui->comboBox_5->setCurrentText(ui->tableWidget_4->item(item->row(),1)->text());
}

/*************************************
 * 函数名：     saveUser
 * 说明：
*************************************/
void parameterUser::saveUser()
{
    QString name = ui->lineEdit->text();
    if(name.isEmpty())
        return;

    QString pwd = ui->lineEdit_11->text();
    QString cPwd = ui->lineEdit_12->text();

    if(pwd != cPwd)
    {
        QMessageBox box(QMessageBox::Question,tr("修改密码"),tr("密码不一致，请重新输入!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        setUserPassword();
        return;
    }

    int uLev = ui->comboBox_5->currentIndex();

    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
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
        QMessageBox box(QMessageBox::Question,tr("用户管理"),tr("Database error!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    QSqlQuery query;

    query.exec(QString("select count(*) from UserList where User='%1';").arg(name));
    query.next();

    int rec = query.value(0).toInt();
    QString sql;
    query.clear();

    if(rec == 0)
    {
        sql = QString("insert into UserList (User,Password,ExpTime,Active,userLevel) VALUES('%1','%2','%3',%4,%5);")
                .arg(name).arg(pwd).arg("2099-12-30").arg(1).arg(uLev);
    }else{
        sql = QString("update UserList set Password='%1',ExpTime='%2',Active=%3,userLevel=%4 where User='%5';")
                .arg(pwd).arg("2099-12-30").arg(1).arg(uLev).arg(name);
    }
    ok = query.exec(sql);
    if(ok)
    {
        QMessageBox box(QMessageBox::Question,tr("用户管理"),tr("保存成功!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
    }
    db.close();
    ui->lineEdit_11->setEnabled(false);
    ui->lineEdit_11->setText("");
    ui->lineEdit_12->setEnabled(false);
    ui->lineEdit_12->setText("");
    ui->lineEdit->setReadOnly(true);
}

/*************************************
 * 函数名：     setUserPassword
 * 说明：
*************************************/
void parameterUser::setUserPassword()
{
    ui->lineEdit_11->setEnabled(true);
    ui->lineEdit_11->setText("");
    ui->lineEdit_12->setEnabled(false);
    ui->lineEdit_12->setText("");
    ui->lineEdit_11->setFocus();
}

/*************************************
 * 函数名：     addUser
 * 说明：
*************************************/
void parameterUser::addUser()
{
    newUser = 1;

    ui->lineEdit_11->setEnabled(false);
    ui->lineEdit_11->setText("");
    ui->lineEdit_12->setEnabled(false);
    ui->lineEdit_12->setText("");

    ui->lineEdit->setEnabled(true);
    ui->lineEdit->setText("");
    ui->lineEdit->setReadOnly(false);
    ui->lineEdit->setFocus();
    ui->comboBox_5->setCurrentIndex(0);
}

/*************************************
 * 函数名：     confirmPassword
 * 说明：
*************************************/
void parameterUser::confirmPassword()
{
    if(ui->lineEdit_11->hasFocus())
    {
        ui->lineEdit_12->setEnabled(true);
        ui->lineEdit_12->setText("");
        ui->lineEdit_12->setFocus();
    }
}

/*************************************
 * 函数名：     savePassword
 * 说明：
*************************************/
void parameterUser::savePassword()
{
    if(!ui->lineEdit_12->hasFocus())
        return;
    saveUser();
}

/*************************************
 * 函数名：     deleteUser
 * 说明：
*************************************/
void parameterUser::deleteUser()
{
    int row = ui->tableWidget_4->currentRow();
    if(row < 0)
        return;

    QString name = ui->tableWidget_4->item(row,0)->text();
    if(name.isEmpty() || name == tc.userName)
        return;

    QMessageBox box(QMessageBox::Question,tr("删除用户"),tr("确定删除选中的用户吗?"));
    box.setWindowFlags(Qt::WindowStaysOnTopHint);
    box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Ok,tr("确 定"));
    box.setButtonText(QMessageBox::Cancel,tr("取 消"));
    int ret = box.exec();
    if(ret == QMessageBox::Cancel)
        return;

    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
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
        QMessageBox messageBox;
        messageBox.setText("Database error");
        messageBox.exec();
        return;
    }

    QSqlQuery query;
    QSqlError error;
    bool ex = query.exec(QString("delete * from UserList where User='%1';").arg(name));
    if(!ex)
    {
//        error = db.lastError();
//        qDebug()<<error.text();
    }else{
        ui->tableWidget_4->removeRow(row);
    }

    db.close();
}
