#include "parameterfile.h"
#include "ui_parameterfile.h"

parameterFile::parameterFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameterFile)
{
    ui->setupUi(this);
    readParameter();
    connect(ui->toolButton_11,SIGNAL(clicked(bool)),this,SLOT(deleteFile()));
    connect(ui->toolButton_12,SIGNAL(clicked(bool)),this,SLOT(checkAll()));
    connect(ui->toolButton_10,SIGNAL(clicked(bool)),this,SLOT(importFiles()));
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitFile()));
}

parameterFile::~parameterFile()
{
    delete ui;
}

void parameterFile::exitFile()
{
    this->close();
    this->deleteLater();
}

/*************************************
 * 函数名：     checkAll
 * 说明：
*************************************/
void parameterFile::checkAll()
{
    Qt::CheckState j;
    if(ui->toolButton_12->text() == tr("全选"))
    {
        j = Qt::Checked;
        ui->toolButton_12->setText(tr("取消全选"));
    }else if(ui->toolButton_12->text() == tr("取消全选")){
        j = Qt::Unchecked;
        ui->toolButton_12->setText(tr("全选"));
    }

    for(int i=0;i<ui->tableWidget_2->rowCount();i++)
    {
        ui->tableWidget_2->item(i,3)->setCheckState(j);
    }
}

/*************************************
 * 函数名：     importFiles
 * 说明：
*************************************/
void parameterFile::importFiles()
{
    QStringList file_name = QFileDialog::getOpenFileNames(this,tr("批量导入"),".","*.txt");

    if(file_name.count() == 0)
        return;

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

    for(int i=0;i<file_name.count();i++)
    {
        QString fileName = file_name.at(i);

        if(!QFile::exists(fileName))
            continue;

        QFileInfo fileinfo;
        fileinfo = QFileInfo(fileName);
        QString name = fileinfo.fileName();

        QSettings *configIniRead = new QSettings(fileName, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

        QStringList it;
        QStringList group = configIniRead->childGroups();
        if(group.count() != 2 || group.at(0) != "BaseInfo" || group.at(1) != "ItemList")
        {
            delete configIniRead;
            continue;
        }

        group.clear();
        configIniRead->beginGroup("BaseInfo");
        group = configIniRead->childKeys();
        it<<QString::number(group.indexOf("GroupNum"),10);
        it<<QString::number(group.indexOf("ItemNum"),10);
        it<<QString::number(group.indexOf("IDCode"),10);
        it<<QString::number(group.indexOf("UpTime"),10);
        it<<QString::number(group.indexOf("NG"),10);
        it<<QString::number(group.indexOf("Freq"),10);
        it<<QString::number(group.indexOf("GNDType"),10);
        it<<QString::number(group.indexOf("DateTime"),10);
        configIniRead->endGroup();

        if(it.indexOf("-1") >= 0)
        {
            delete configIniRead;
            continue;
        }
        it.clear();

        group.clear();
        configIniRead->beginGroup("ItemList");
        group = configIniRead->childKeys();
        configIniRead->endGroup();

        if(group.count() == 0)
        {
            delete configIniRead;
            continue;
        }

        int ok = 0;
        for(int i=1;i<=group.count();i++)
        {
            ok = group.indexOf(QString("item%1").arg(i));
            if(ok == -1)
                break;
        }

        if(ok == -1)
        {
            delete configIniRead;
            continue;
        }

        int index;
        index = name.lastIndexOf(".");
        name.truncate(index);
        index = 0;
        QSqlQuery q;
        QString sql;
        while(1)
        {
            QString a;
            if(index > 0)
               a = "_"+QString::number(index,10);

            sql = "select count(*) from TestProgram where TPName='"+name+a+"';";
            q.exec(sql);
            q.next();
            if(q.value(0).toInt() == 0)
            {
                name += a;
                break;
            }
            q.clear();
            index++;
        }
        configIniRead->beginGroup("BaseInfo");
        int groupNum = configIniRead->value("GroupNum").toInt();
        QString IDCode = configIniRead->value("IDCode").toString();
        QString saveTime = configIniRead->value("DateTime").toString();
        configIniRead->endGroup();

        q.prepare("insert into TestProgram (ID,TPName,TPTime,IDCode) VALUES(:v1,:v2,:v3,:v4);");
        q.bindValue(":v1",groupNum);
        q.bindValue(":v2",name);
        q.bindValue(":v3",saveTime);
        q.bindValue(":v4",IDCode);
        bool isSucceed = q.exec();
        if(!isSucceed)
        {
            QSqlError se = q.lastError();
            QMessageBox messageBox;
            messageBox.setText(se.text());
            messageBox.exec();
        }

        QFile::copy(fileName,QCoreApplication::applicationDirPath()+"/files/"+name+".txt");

        delete configIniRead;
    }
    db.close();
    readParameter();
}

/*************************************
 * 函数名：     deleteFile
 * 说明：
*************************************/
void parameterFile::deleteFile()
{
    if(ui->tableWidget_2->rowCount() == 0)
        return;

    QMessageBox box(QMessageBox::Question,tr("删除测试程序"),tr("确定删除选中的测试程序吗?"));
    box.setWindowFlags(Qt::WindowStaysOnTopHint);
    box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    box.setButtonText(QMessageBox::Ok,QString(tr("确 定")));
    box.setButtonText(QMessageBox::Cancel,QString(tr("取 消")));
    int ret = box.exec();
    if(ret == QMessageBox::Cancel)
        return;

    //连接数据库
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
        return;
    }

    for(int i=0;i<ui->tableWidget_2->rowCount();i++)
    {
        if(ui->tableWidget_2->item(i,3)->checkState() == Qt::Unchecked)
            continue;

        QSqlQuery q;
        QSqlError error;
        QString sql = "delete * from TestProgram where TPName='"+ui->tableWidget_2->item(i,1)->text()+"';";

        bool ex = q.exec(sql);
        if(!ex)
        {
            error = db.lastError();
//            qDebug()<<error.text();
        }else{
            QFile::remove(QCoreApplication::applicationDirPath()+"/files/"+ui->tableWidget_2->item(i,1)->text()+".txt");
        }
    }
    db.close();
    readParameter();
}

void parameterFile::chooseFile(QModelIndex index)
{
    if(ui->tableWidget_2->item(index.row(),3)->checkState() == Qt::Unchecked)
        ui->tableWidget_2->item(index.row(),3)->setCheckState(Qt::Checked);
    else
        ui->tableWidget_2->item(index.row(),3)->setCheckState(Qt::Unchecked);
}

void parameterFile::readParameter()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->setColumnWidth(0,40);
    ui->tableWidget_2->setColumnWidth(1,400);
    ui->tableWidget_2->setColumnWidth(2,200);
    ui->tableWidget_2->setColumnWidth(3,40);

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
    disconnect(ui->tableWidget_2,SIGNAL(clicked(QModelIndex)),this,SLOT(chooseFile(QModelIndex)));

    QSqlQuery q;
    QString sql = "select ID,TPName,TPTime,IDCode from TestProgram order by ID";
    q.exec(sql);

    int row = 0;
    while(q.next())
    {
        ui->tableWidget_2->insertRow(row);

        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFont(font);
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(q.value(0).toString());
        ui->tableWidget_2->setItem(row,0,item);

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setFont(font);
        item1->setTextAlignment(Qt::AlignCenter);
        item1->setText(q.value(1).toString());
        ui->tableWidget_2->setItem(row,1,item1);

        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setFont(font);
        item2->setTextAlignment(Qt::AlignCenter);
        item2->setText(q.value(2).toString());
        ui->tableWidget_2->setItem(row,2,item2);

        QTableWidgetItem *check = new QTableWidgetItem();
        check->setCheckState(Qt::Unchecked);
        check->setText("");
        check->setFlags(check->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget_2->setItem(row,3,check);

        row++;
    }
    db.close();
    connect(ui->tableWidget_2,SIGNAL(clicked(QModelIndex)),this,SLOT(chooseFile(QModelIndex)));
}
