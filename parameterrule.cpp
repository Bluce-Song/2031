#include "parameterrule.h"
#include "ui_parameterrule.h"

parameterRule::parameterRule(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameterRule)
{
    ui->setupUi(this);
    readParameter();
    connect(ui->tableWidget_3,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(readEncodingRules(QTableWidgetItem*)));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(deleteEncodingRules()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(saveEncodingRules()));
    connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(setMask(QTableWidgetItem*)));
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitRule()));
}

parameterRule::~parameterRule()
{
    delete ui;
}

void parameterRule::exitRule()
{
    this->close();
    this->deleteLater();
}

void parameterRule::readParameter()
{
    int width = ui->tableWidget_3->width();
    ui->tableWidget_3->setColumnWidth(0,width/17);
    ui->tableWidget_3->setColumnWidth(1,width*2/17);
    ui->tableWidget_3->setColumnWidth(2,width*12/17);
    ui->tableWidget_3->setColumnWidth(3,width*2/17);
    disconnect(ui->spinBox_6,SIGNAL(valueChanged(int)),this,SLOT(setEncodingRulesLen(int)));
    disconnect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
    getAllEncodingRules(0);
    connect(ui->spinBox_6,SIGNAL(valueChanged(int)),this,SLOT(setEncodingRulesLen(int)));
    connect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
}

/*************************************
 * 函数名：     setEncodingRulesLen
 * 说明：
*************************************/
void parameterRule::setEncodingRulesLen(int len)
{
    QFont font("Microsoft YaHei",12,QFont::Normal);
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(len);
    for(int i=0;i<len;i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFont(font);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
        item->setText("");
        ui->tableWidget->setItem(0,i,item);
    }
}

/*************************************
 * 函数名：     saveEncodingRules
 * 说明：
*************************************/
void parameterRule::saveEncodingRules()
{
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QODBC");
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);
    db.setDatabaseName(dsn);    //设置连接字符串
    db.setUserName("");     //设置登陆数据库的用户名
    db.setPassword("");     //设置密码
    bool ok = db.open();

    if(!ok)
    {
        return;
    }

    int num = ui->spinBox_7->value();
    int len = ui->spinBox_6->value();
    int uLen = 0;
    int ac = -1;

    if(ui->checkBox_20->checkState() == Qt::Checked)
        ac = 1;
    else
        ac = 0;

    QString data = NULL;
    for(int i=0;i<len;i++)
    {
        if(ui->tableWidget->item(0,i)->text() == "X")
        {
            data += "X";
            uLen++;
        }else{
            data += "_";
        }
    }

    QSqlQuery q;

    QString sql = "select count(*) from EncodingRules where Data='"+data+"';";

    q.exec(sql);
    q.next();
    int rec = q.value(0).toInt();
    q.clear();

    if(rec == 0)
    {
        q.exec(QString("select count(*) from EncodingRules where Index=%1").arg(num));
        q.next();
        int nn = q.value(0).toInt();
        q.clear();

        if(nn == 0)
        {
            sql = QString("insert into EncodingRules (Index,Length,Data,Active,UseLength) VALUES(%1,%2,'%3',%4,%5);")
                    .arg(num).arg(len).arg(data).arg(ac).arg(uLen);
        }else{
            sql = QString("update EncodingRules set Length=%1,Data='%2',Active=%3,UseLength=%4 where Index=%5;")
                    .arg(len).arg(data).arg(ac).arg(uLen).arg(num);
        }
    }else if(rec == 1){
        q.exec(QString("select Index from EncodingRules where Data='%1'").arg(data));
        q.next();
        int index = q.value(0).toInt();
        q.clear();

        if(index != num)
        {
            db.close();
            QMessageBox box(QMessageBox::Warning,tr("编码规则"),tr("编码规则已存在，请重新编辑!"));
            box.setWindowFlags(Qt::WindowStaysOnTopHint);
            box.setStandardButtons(QMessageBox::Ok);
            box.setButtonText(QMessageBox::Ok,tr("确 定"));
            box.exec();
            return;
        }

        sql = QString("update EncodingRules set Length=%1,Data='%2',Active=%3,UseLength=%4 where Index=%5;")
                .arg(len).arg(data).arg(ac).arg(uLen).arg(num);
    }
    ok = q.exec(sql);
    q.clear();

    db.close();
    disconnect(ui->spinBox_6,SIGNAL(valueChanged(int)),this,SLOT(setEncodingRulesLen(int)));
    disconnect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
    getAllEncodingRules(1);
    connect(ui->spinBox_6,SIGNAL(valueChanged(int)),this,SLOT(setEncodingRulesLen(int)));
    connect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
}

/*************************************
 * 函数名：     deleteEncodingRules
 * 说明：
*************************************/
void parameterRule::deleteEncodingRules()
{
    int num = ui->spinBox_7->value();
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
    QSqlError error;
//    QString sql = "delete * from EncodingRules where Index="+num+";";
    bool ex = q.exec(QString("delete * from EncodingRules where Index=%1;").arg(num));
    if(!ex)
    {
        error = db.lastError();
        qDebug()<<error.text();
    }

    db.close();
    disconnect(ui->spinBox_6,SIGNAL(valueChanged(int)),this,SLOT(setEncodingRulesLen(int)));
    disconnect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
    getAllEncodingRules(0);
    connect(ui->spinBox_6,SIGNAL(valueChanged(int)),this,SLOT(setEncodingRulesLen(int)));
    connect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
}

/*************************************
 * 函数名：     setMask
 * 说明：
*************************************/
void parameterRule::setMask(QTableWidgetItem* item)
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    QTableWidgetItem *it = new QTableWidgetItem();
    it->setFont(font);
    it->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    if(ui->tableWidget->item(0,item->column())->text() == "X")
    {
        it->setText("_");
    }else{
        it->setText("X");
    }
    ui->tableWidget->setItem(0,item->column(),it);
}

/*************************************
 * 函数名：     readEncodingRules
 * 说明：
*************************************/
void parameterRule::readEncodingRules(QTableWidgetItem* item)
{
    disconnect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
    readEncodingRules(ui->tableWidget_3->item(item->row(),0)->text());
    connect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
}

/*************************************
 * 函数名：     readEncodingRules
 * 说明：
*************************************/
void parameterRule::readEncodingRules(QString num)
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

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
    QString sql = "select * from EncodingRules where Index="+num+";";
    q.exec(sql);
    int index = 0;
    while(q.next())
    {
        index++;
        int len = q.value(1).toInt();
        QString v = q.value(2).toString();

        ui->tableWidget->setRowCount(1);
        ui->tableWidget->setColumnCount(len);

        ui->tableWidget->clear();
        ui->spinBox_7->setValue(q.value(0).toInt());
        ui->spinBox_6->setValue(len);

        if(q.value(3).toInt() == 1)
            ui->checkBox_20->setCheckState(Qt::Checked);
        else
            ui->checkBox_20->setCheckState(Qt::Unchecked);

        for(int i=0;i<len;i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFont(font);
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
            item->setText(v.mid(i,1));
            ui->tableWidget->setItem(0,i,item);
        }
    }
    q.clear();
    db.close();

    if(index == 0)
    {
        ui->spinBox_6->setValue(10);
        ui->checkBox_20->setCheckState(Qt::Unchecked);
//        ui->tableWidget->clear();
        for(int i=0;i<10;i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
            item->setText("");
            ui->tableWidget->setItem(0,i,item);
        }
    }
}

/*************************************
 * 函数名：     getAllEncodingRules
 * 说明：
*************************************/
void parameterRule::getAllEncodingRules(int index)
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

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
    QSqlError error;
    bool ex = q.exec("select * from EncodingRules order by Index ASC;");
    if(!ex)
    {
        error = db.lastError();
//        qDebug()<<error.text();
    }
    int row = 0;
    ui->tableWidget_3->setRowCount(0);
    while(q.next())
    {
        ui->tableWidget_3->insertRow(row);

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFont(font);
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(q.value(0).toString());
        ui->tableWidget_3->setItem(row,0,item);

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setFont(font);
        item1->setTextAlignment(Qt::AlignCenter);
        item1->setText(q.value(1).toString());
        ui->tableWidget_3->setItem(row,1,item1);

        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setFont(font);
        item2->setTextAlignment(Qt::AlignCenter);
        item2->setText(q.value(2).toString());
        ui->tableWidget_3->setItem(row,2,item2);

        QTableWidgetItem *item3 = new QTableWidgetItem();
        item3->setFont(font);
        item3->setTextAlignment(Qt::AlignCenter);

        if(q.value(3).toInt() == 1)
            item3->setText("√");
        else
            item3->setText("×");
        ui->tableWidget_3->setItem(row,3,item3);

        if(row == 0 && index == 0)
        {
            int len = q.value(1).toInt();
            QString v = q.value(2).toString();

            ui->tableWidget->setRowCount(1);
            ui->tableWidget->setColumnCount(len);

            ui->tableWidget->clear();
            ui->spinBox_7->setValue(q.value(0).toInt());
            ui->spinBox_6->setValue(len);

            if(q.value(3).toInt() == 1)
                ui->checkBox_20->setCheckState(Qt::Checked);
            else
                ui->checkBox_20->setCheckState(Qt::Unchecked);

            for(int i=0;i<len;i++)
            {
                QTableWidgetItem *item4 = new QTableWidgetItem();
                item4->setFont(font);
                item4->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
                item4->setText(v.mid(i,1));
                ui->tableWidget->setItem(0,i,item4);
            }
        }
        row++;
    }
    q.clear();
    db.close();
}
