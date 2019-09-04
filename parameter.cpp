#include "parameter.h"
#include "ui_parameter.h"
#include "test_main.h"

parameter::parameter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameter)
{
    ui->setupUi(this);

    ui->label_44->setVisible(false);
    ui->lineEdit_20->setVisible(false);

    QListWidgetItem *it = ui->listWidget->takeItem(4);
    it->setHidden(true);
    QListWidgetItem *it1 = ui->listWidget->takeItem(4);
    it1->setHidden(true);

    actionA = new QAction(this);
    actionA->setShortcut(QKeySequence("Meta+Alt+A"));
    this->addAction(actionA);

    connect(actionA,SIGNAL(triggered()),this,SLOT(showPasswordItem()));
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitParameter()));
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(setStackedPage(int)));    
    connect(ui->tableWidget_2,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(checkData(QTableWidgetItem*)));
    connect(ui->toolButton_11,SIGNAL(clicked(bool)),this,SLOT(deleteFile()));
    connect(ui->toolButton_12,SIGNAL(clicked(bool)),this,SLOT(checkAll()));
    connect(ui->toolButton_10,SIGNAL(clicked(bool)),this,SLOT(importFiles()));    
    connect(ui->tableWidget_3,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(readEncodingRules(QTableWidgetItem*)));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(deleteEncodingRules()));
    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(saveEncodingRules()));
    connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(setMask(QTableWidgetItem*)));    
}

parameter::~parameter()
{
    delete ui;
}

void parameter::hiddenPasswordItem()
{
    ui->label_44->setVisible(false);
    ui->lineEdit_20->setVisible(false);
    connect(actionA,SIGNAL(triggered()),this,SLOT(showPasswordItem()));
    disconnect(actionA,SIGNAL(triggered()),this,SLOT(hiddenPasswordItem()));
    disconnect(ui->lineEdit_20,SIGNAL(returnPressed()),this,SLOT(on_loginOK_clicked()));
}

void parameter::showPasswordItem()
{
    ui->label_44->setVisible(true);
    ui->lineEdit_20->setVisible(true);
    ui->lineEdit_20->setFocus();
    ui->lineEdit_20->setText("");
    disconnect(actionA,SIGNAL(triggered()),this,SLOT(showPasswordItem()));
    connect(actionA,SIGNAL(triggered()),this,SLOT(hiddenPasswordItem()));
    connect(ui->lineEdit_20,SIGNAL(returnPressed()),this,SLOT(on_loginOK_clicked()));
}

void parameter::on_loginOK_clicked()
{
    QString pw = ui->lineEdit_20->text();
    if(pw == "Oking999")
    {
        ui->label_44->setVisible(false);
        ui->lineEdit_20->setVisible(false);
        QFont font("Microsoft YaHei",16,QFont::Normal);
        ui->listWidget->addItem(tr("量程设置"));
        ui->listWidget->addItem(tr("高级"));
        ui->listWidget->setFont(font);
        disconnect(actionA,SIGNAL(triggered()),this,SLOT(showPasswordItem()));
        disconnect(actionA,SIGNAL(triggered()),this,SLOT(hiddenPasswordItem()));
        disconnect(ui->lineEdit_20,SIGNAL(returnPressed()),this,SLOT(on_loginOK_clicked()));
    }
}

void parameter::setStackedPage(int index)
{        
    if(tc.userLevel == 0 && index >= 3)
    {
        QMessageBox box(QMessageBox::Warning,tr("参数设置"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    readParameter(index);
    ui->stackedWidget->setCurrentIndex(index);
}

/*************************************
 * 函数名：     checkItem
 * 说明：
*************************************/
void parameter::checkItem(QTableWidgetItem* item)
{
    int row = item->row();
    int column = item->column();

    if(column == 0 && (row == 0 || row == 3 || row == 6 || row == 9 || row == 13 || row == 16 || row == 20 || row == 24))
    {
        if(item->checkState() == Qt::Checked)
            item->setCheckState(Qt::Unchecked);
        else
            item->setCheckState(Qt::Checked);
    }
}

/*************************************
 * 函数名：     saveEncodingRules
 * 说明：
*************************************/
void parameter::saveEncodingRules()
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
 * 函数名：     setMask
 * 说明：
*************************************/
void parameter::setMask(QTableWidgetItem* item)
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
 * 函数名：     deleteEncodingRules
 * 说明：
*************************************/
void parameter::deleteEncodingRules()
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
 * 函数名：     setEncodingRulesLen
 * 说明：
*************************************/
void parameter::setEncodingRulesLen(int len)
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
 * 函数名：     readEncodingRules
 * 说明：
*************************************/
void parameter::readEncodingRules(QTableWidgetItem* item)
{
    disconnect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
    readEncodingRules(ui->tableWidget_3->item(item->row(),0)->text());
    connect(ui->spinBox_7,SIGNAL(valueChanged(QString)),this,SLOT(readEncodingRules(QString)));
}

/*************************************
 * 函数名：     readEncodingRules
 * 说明：
*************************************/
void parameter::readEncodingRules(QString num)
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
void parameter::getAllEncodingRules(int index)
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

/*************************************
 * 函数名：     readUserInfo
 * 说明：
*************************************/
void parameter::readUserInfo(QTableWidgetItem* item)
{
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit->setText(ui->tableWidget_4->item(item->row(),0)->text());
    ui->comboBox_5->setCurrentText(ui->tableWidget_4->item(item->row(),1)->text());
}

/*************************************
 * 函数名：     importFiles
 * 说明：
*************************************/
void parameter::importFiles()
{
    QStringList file_name = QFileDialog::getOpenFileNames(this,tr("批量导入"),".","*.txt");

    if(file_name.count() == 0)
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
    readParameter(1);
}

/*************************************
 * 函数名：     deleteFile
 * 说明：
*************************************/
void parameter::deleteFile()
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
    readParameter(1);
}

/*************************************
 * 函数名：     checkAll
 * 说明：
*************************************/
void parameter::checkAll()
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
 * 函数名：     checkData
 * 说明：
*************************************/
void parameter::checkData(QTableWidgetItem* item)
{
    if(item->column() == 3)
    {
        if(item->checkState() == Qt::Checked)
            item->setCheckState(Qt::Unchecked);
        else
            item->setCheckState(Qt::Checked);

        ui->tableWidget_2->setItem(item->row(),item->column(),item);
    }
}

/*************************************
 * 函数名：     checkData
 * 说明：
*************************************/
void parameter::checkData(int row,int column)
{
    changed = 1;
    QString t = ui->tableWidget_5->item(row,column)->text();

    if(column == 7 || column == 8)
    {
        QRegExp rx("^\\d+$");
        if(!rx.exactMatch(t))
        {
            disconnect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            ui->tableWidget_5->item(row,column)->setText("0");
            connect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
        }else{
            int prec = ui->tableWidget_5->item(row,8)->text().toInt();
            int prec2 = ui->tableWidget_5->item(row,7)->text().toInt();

            if(prec2 > prec)
            {
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("默认精度不能大于精度设定值！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();

                ui->tableWidget_5->item(row,column)->setText("0");
                return;
            }

            if(column == 8)
            {
                if(!ui->tableWidget_5->item(row,2)->text().isEmpty())
                    ui->tableWidget_5->item(row,2)->setText(QString::number(ui->tableWidget_5->item(row,2)->text().toFloat(),10,prec));

                if(!ui->tableWidget_5->item(row,3)->text().isEmpty())
                    ui->tableWidget_5->item(row,3)->setText(QString::number(ui->tableWidget_5->item(row,3)->text().toFloat(),10,prec));

                if(!ui->tableWidget_5->item(row,4)->text().isEmpty())
                    ui->tableWidget_5->item(row,4)->setText(QString::number(ui->tableWidget_5->item(row,4)->text().toFloat(),10,prec));

                if(!ui->tableWidget_5->item(row,5)->text().isEmpty())
                    ui->tableWidget_5->item(row,5)->setText(QString::number(ui->tableWidget_5->item(row,5)->text().toFloat(),10,prec));

                if(!ui->tableWidget_5->item(row,6)->text().isEmpty())
                    ui->tableWidget_5->item(row,6)->setText(QString::number(ui->tableWidget_5->item(row,6)->text().toFloat(),10,prec));
            }
        }
    }else if(column >= 2 && column <= 6){
        QRegExp rx("^\\d+\(\\.\\d+\)?$");
        int prec1 = ui->tableWidget_5->item(row,8)->text().toInt();
        if(!rx.exactMatch(t))
        {
            disconnect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            ui->tableWidget_5->item(row,column)->setText(QString::number(0.0,10,prec1));
            connect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
        }else{
            disconnect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            ui->tableWidget_5->item(row,column)->setText(QString::number(t.toFloat(),10,prec1));
            connect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
        }
    }
}

/*************************************
 * 函数名：     saveRange
 * 说明：
*************************************/
void parameter::saveRange()
{
    range_Item set[8];
    ui->tableWidget_5->setFocus();

    for(int i=0;i<8;i++)
        for(int j=0;j<3;j++)
        {
            set[i].def[j] = ui->tableWidget_5->item(i*3+j,2)->text().toFloat();
            set[i].minDefault[j] = ui->tableWidget_5->item(i*3+j,3)->text().toFloat();
            set[i].min[j] = ui->tableWidget_5->item(i*3+j,4)->text().toFloat();
            set[i].maxDefault[j] = ui->tableWidget_5->item(i*3+j,5)->text().toFloat();
            set[i].max[j] = ui->tableWidget_5->item(i*3+j,6)->text().toFloat();
            set[i].precDefault[j] = ui->tableWidget_5->item(i*3+j,7)->text().toInt();
            set[i].prec[j] = ui->tableWidget_5->item(i*3+j,8)->text().toInt();

            QWidget *widget = ui->tableWidget_5->cellWidget(i*3+j,9);
            QComboBox *combox = (QComboBox*)widget;
            set[i].unit[j] = combox->currentText();

            if(set[i].min[j] > set[i].max[j])
            {
                ui->tableWidget_5->item(i*3+j,4)->setSelected(true);
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("下限大于上限！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }

            if(set[i].def[j] < set[i].min[j])
            {
                ui->tableWidget_5->item(i*3+j,2)->setSelected(true);
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("默认值低于下限！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }

            if(set[i].def[j] > set[i].max[j])
            {
                ui->tableWidget_5->item(i*3+j,2)->setSelected(true);
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("默认值大于上限！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }

            if(set[i].minDefault[j] > set[i].max[j])
            {
                ui->tableWidget_5->item(i*3+j,3)->setSelected(true);
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("默认下限大于上限！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }

            if(set[i].minDefault[j] < set[i].min[j])
            {
                ui->tableWidget_5->item(i*3+j,3)->setSelected(true);
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("默认下限小于下限！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }

            if(set[i].maxDefault[j] > set[i].max[j])
            {
                ui->tableWidget_5->item(i*3+j,5)->setSelected(true);
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("默认上限大于上限！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }

            if(set[i].maxDefault[j] < set[i].min[j])
            {
                ui->tableWidget_5->item(i*3+j,5)->setSelected(true);
                QMessageBox box(QMessageBox::Warning,tr("量程设置"),tr("默认上限小于下限！"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return;
            }
        }

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

    QSqlQuery query;
    bool isSucceed;

    for(int i=0;i<8;i++)
    {
        int r;
        if(i == 0 || i == 1 || i == 2 || i == 3)
            r = i*3;
        else if(i == 4 || i == 5)
            r = i*3+1;
        else if(i == 6)
            r = 20;
        else if(i == 7)
            r = 24;

        if(ui->tableWidget_5->item(r,0)->checkState() == Qt::Checked)
            set[i].active = 1;
        else
            set[i].active = 0;

        for(int j=0;j<3;j++)
        {
            int row;
            if(i == 0 || i == 1 || i == 2 || i == 3)
                row = i*3+j;
            else if(i == 4 || i == 5)
                row = i*3+j+1;
            else if(i == 6)
                row = 20+j;
            else if(i == 7)
                row = 24+j;

            set[i].def[j] = ui->tableWidget_5->item(row,2)->text().toFloat();
            set[i].minDefault[j] = ui->tableWidget_5->item(row,3)->text().toFloat();
            set[i].min[j] = ui->tableWidget_5->item(row,4)->text().toFloat();
            set[i].maxDefault[j] = ui->tableWidget_5->item(row,5)->text().toFloat();
            set[i].max[j] = ui->tableWidget_5->item(row,6)->text().toFloat();
            set[i].precDefault[j] = ui->tableWidget_5->item(row,7)->text().toInt();
            set[i].prec[j] = ui->tableWidget_5->item(row,8)->text().toInt();

            QWidget *widget = ui->tableWidget_5->cellWidget(row,9);
            QComboBox *combox = (QComboBox*)widget;
            set[i].unit[j] = combox->currentText();
        }

        if(i == 3)  //接地
        {
            set[i].def[3] = ui->tableWidget_5->item(12,2)->text().toFloat();
            set[i].minDefault[3] = ui->tableWidget_5->item(12,3)->text().toFloat();
            set[i].min[3] = ui->tableWidget_5->item(12,4)->text().toFloat();
            set[i].maxDefault[3] = ui->tableWidget_5->item(12,5)->text().toFloat();
            set[i].max[3] = ui->tableWidget_5->item(12,6)->text().toFloat();
            set[i].precDefault[3] = ui->tableWidget_5->item(12,7)->text().toInt();
            set[i].prec[3] = ui->tableWidget_5->item(12,8)->text().toInt();

            QWidget *widget = ui->tableWidget_5->cellWidget(12,9);
            QComboBox *combox = (QComboBox*)widget;
            set[i].unit[3] = combox->currentText();
        }

        if(i == 5 || i == 6)
        {
            set[i].def[3] = ui->tableWidget_5->item(i*3+3+(i==6?2:1),2)->text().toFloat();
            set[i].minDefault[3] = ui->tableWidget_5->item(i*3+3+(i==6?2:1),3)->text().toFloat();
            set[i].min[3] = ui->tableWidget_5->item(i*3+3+(i==6?2:1),4)->text().toFloat();
            set[i].maxDefault[3] = ui->tableWidget_5->item(i*3+3+(i==6?2:1),5)->text().toFloat();
            set[i].max[3] = ui->tableWidget_5->item(i*3+3+(i==6?2:1),6)->text().toFloat();
            set[i].precDefault[3] = ui->tableWidget_5->item(i*3+3+(i==6?2:1),7)->text().toInt();
            set[i].prec[3] = ui->tableWidget_5->item(i*3+3+(i==6?2:1),8)->text().toInt();

            QWidget *widget = ui->tableWidget_5->cellWidget(i*3+3+(i==6?2:1),9);
            QComboBox *combox = (QComboBox*)widget;
            set[i].unit[3] = combox->currentText();
        }

        query.clear();
        query.prepare("update ItemRange set Active=:v1,vDefault=:v2,vMinDefault=:v3,vMin=:v4,"
                      "vMaxDefault=:v5,vMax=:v6,vFormatDefault=:v7,vFormat=:v8,vUnit=:v9,"
                      "crpDefault=:v10,crpMinDefault=:v11,crpMin=:v12,crpMaxDefault=:v13,"
                      "crpMax=:v14,crpFormatDefault=:v15,crpFormat=:v16,crpUnit=:v17,"
                      "timeDefault=:v18,timeMinDefault=:v19,timeMin=:v20,timeMaxDefault=:v21,"
                      "timeMax=:v22,timeFormatDefault=:v23,timeFormat=:v24,timeUnit=:v25,"
                      "spareDefault=:v26,spareMinDefault=:v27,spareMin=:v28,spareMaxDefault=:v29,"
                      "spareMax=:v30,spareFormatDefault=:v31,spareFormat=:v32,spareUnit=:v33 where ID=:v34");

        if(i == 3)
        {
            query.bindValue(":v1",set[i].active);
            query.bindValue(":v2",set[i].def[1]);
            query.bindValue(":v3",set[i].minDefault[1]);
            query.bindValue(":v4",set[i].min[1]);
            query.bindValue(":v5",set[i].maxDefault[1]);
            query.bindValue(":v6",set[i].max[1]);
            query.bindValue(":v7",set[i].precDefault[1]);
            query.bindValue(":v8",set[i].prec[1]);
            query.bindValue(":v9",set[i].unit[1]);

            query.bindValue(":v10",set[i].def[2]);
            query.bindValue(":v11",set[i].minDefault[2]);
            query.bindValue(":v12",set[i].min[2]);
            query.bindValue(":v13",set[i].maxDefault[2]);
            query.bindValue(":v14",set[i].max[2]);
            query.bindValue(":v15",set[i].precDefault[2]);
            query.bindValue(":v16",set[i].prec[2]);
            query.bindValue(":v17",set[i].unit[2]);

            query.bindValue(":v18",set[i].def[3]);
            query.bindValue(":v19",set[i].minDefault[3]);
            query.bindValue(":v20",set[i].min[3]);
            query.bindValue(":v21",set[i].maxDefault[3]);
            query.bindValue(":v22",set[i].max[3]);
            query.bindValue(":v23",set[i].precDefault[3]);
            query.bindValue(":v24",set[i].prec[3]);
            query.bindValue(":v25",set[i].unit[3]);

            query.bindValue(":v26",set[i].def[0]);
            query.bindValue(":v27",set[i].minDefault[0]);
            query.bindValue(":v28",set[i].min[0]);
            query.bindValue(":v29",set[i].maxDefault[0]);
            query.bindValue(":v30",set[i].max[0]);
            query.bindValue(":v31",set[i].precDefault[0]);
            query.bindValue(":v32",set[i].prec[0]);
            query.bindValue(":v33",set[i].unit[0]);

            query.bindValue(":v34",i+1);

        }else if(i == 5){
            query.bindValue(":v1",set[i].active);
            query.bindValue(":v2",set[i].def[0]);
            query.bindValue(":v3",set[i].minDefault[0]);
            query.bindValue(":v4",set[i].min[0]);
            query.bindValue(":v5",set[i].maxDefault[0]);
            query.bindValue(":v6",set[i].max[0]);
            query.bindValue(":v7",set[i].precDefault[0]);
            query.bindValue(":v8",set[i].prec[0]);
            query.bindValue(":v9",set[i].unit[0]);

            query.bindValue(":v10",set[i].def[2]);
            query.bindValue(":v11",set[i].minDefault[2]);
            query.bindValue(":v12",set[i].min[2]);
            query.bindValue(":v13",set[i].maxDefault[2]);
            query.bindValue(":v14",set[i].max[2]);
            query.bindValue(":v15",set[i].precDefault[2]);
            query.bindValue(":v16",set[i].prec[2]);
            query.bindValue(":v17",set[i].unit[2]);

            query.bindValue(":v18",set[i].def[3]);
            query.bindValue(":v19",set[i].minDefault[3]);
            query.bindValue(":v20",set[i].min[3]);
            query.bindValue(":v21",set[i].maxDefault[3]);
            query.bindValue(":v22",set[i].max[3]);
            query.bindValue(":v23",set[i].precDefault[3]);
            query.bindValue(":v24",set[i].prec[3]);
            query.bindValue(":v25",set[i].unit[3]);

            query.bindValue(":v26",set[i].def[1]);
            query.bindValue(":v27",set[i].minDefault[1]);
            query.bindValue(":v28",set[i].min[1]);
            query.bindValue(":v29",set[i].maxDefault[1]);
            query.bindValue(":v30",set[i].max[1]);
            query.bindValue(":v31",set[i].precDefault[1]);
            query.bindValue(":v32",set[i].prec[1]);
            query.bindValue(":v33",set[i].unit[1]);

            query.bindValue(":v34",i+1);

        }else if(i == 6){

            query.bindValue(":v1",set[i].active);
            query.bindValue(":v2",set[i].def[0]);
            query.bindValue(":v3",set[i].minDefault[0]);
            query.bindValue(":v4",set[i].min[0]);
            query.bindValue(":v5",set[i].maxDefault[0]);
            query.bindValue(":v6",set[i].max[0]);
            query.bindValue(":v7",set[i].precDefault[0]);
            query.bindValue(":v8",set[i].prec[0]);
            query.bindValue(":v9",set[i].unit[0]);

            query.bindValue(":v10",set[i].def[1]);
            query.bindValue(":v11",set[i].minDefault[1]);
            query.bindValue(":v12",set[i].min[1]);
            query.bindValue(":v13",set[i].maxDefault[1]);
            query.bindValue(":v14",set[i].max[1]);
            query.bindValue(":v15",set[i].precDefault[1]);
            query.bindValue(":v16",set[i].prec[1]);
            query.bindValue(":v17",set[i].unit[1]);

            query.bindValue(":v18",set[i].def[3]);
            query.bindValue(":v19",set[i].minDefault[3]);
            query.bindValue(":v20",set[i].min[3]);
            query.bindValue(":v21",set[i].maxDefault[3]);
            query.bindValue(":v22",set[i].max[3]);
            query.bindValue(":v23",set[i].precDefault[3]);
            query.bindValue(":v24",set[i].prec[3]);
            query.bindValue(":v25",set[i].unit[3]);

            query.bindValue(":v26",set[i].def[2]);
            query.bindValue(":v27",set[i].minDefault[2]);
            query.bindValue(":v28",set[i].min[2]);
            query.bindValue(":v29",set[i].maxDefault[2]);
            query.bindValue(":v30",set[i].max[2]);
            query.bindValue(":v31",set[i].precDefault[2]);
            query.bindValue(":v32",set[i].prec[2]);
            query.bindValue(":v33",set[i].unit[2]);

            query.bindValue(":v34",i+1);

        }else{

            query.bindValue(":v1",set[i].active);
            query.bindValue(":v2",set[i].def[0]);
            query.bindValue(":v3",set[i].minDefault[0]);
            query.bindValue(":v4",set[i].min[0]);
            query.bindValue(":v5",set[i].maxDefault[0]);
            query.bindValue(":v6",set[i].max[0]);
            query.bindValue(":v7",set[i].precDefault[0]);
            query.bindValue(":v8",set[i].prec[0]);
            query.bindValue(":v9",set[i].unit[0]);

            query.bindValue(":v10",set[i].def[1]);
            query.bindValue(":v11",set[i].minDefault[1]);
            query.bindValue(":v12",set[i].min[1]);
            query.bindValue(":v13",set[i].maxDefault[1]);
            query.bindValue(":v14",set[i].max[1]);
            query.bindValue(":v15",set[i].precDefault[1]);
            query.bindValue(":v16",set[i].prec[1]);
            query.bindValue(":v17",set[i].unit[1]);

            query.bindValue(":v18",set[i].def[2]);
            query.bindValue(":v19",set[i].minDefault[2]);
            query.bindValue(":v20",set[i].min[2]);
            query.bindValue(":v21",set[i].maxDefault[2]);
            query.bindValue(":v22",set[i].max[2]);
            query.bindValue(":v23",set[i].precDefault[2]);
            query.bindValue(":v24",set[i].prec[2]);
            query.bindValue(":v25",set[i].unit[2]);

            query.bindValue(":v26",0.0);
            query.bindValue(":v27",0.0);
            query.bindValue(":v28",0.0);
            query.bindValue(":v29",1.0);
            query.bindValue(":v30",2.0);
            query.bindValue(":v31",1);
            query.bindValue(":v32",1);
            query.bindValue(":v33","unit");

            query.bindValue(":v34",i+1);
        }
        isSucceed = query.exec();
        if(!isSucceed)
        {
            QSqlError se = query.lastError();
//            qDebug()<<"3"<<se.text();
            QMessageBox messageBox;
            messageBox.setText(se.text());
            messageBox.exec();
        }
    }
    query.clear();
    changed = 0;

    QString sql = "select TIName,ID,Active,vDefault,vMinDefault,vMin,vMaxDefault,vMax,vFormatDefault,"
                  "vFormat,vUnit,crpDefault,crpMinDefault,crpMin,crpMaxDefault,crpMax,crpFormatDefault,"
                  "crpFormat,crpUnit,timeDefault,timeMinDefault,timeMin,timeMaxDefault,timeMax,"
                  "timeFormatDefault,timeFormat,timeUnit,spareDefault,spareMinDefault,spareMin,"
                  "spareMaxDefault,spareMax,spareFormatDefault,spareFormat,spareUnit"
                  " from ItemRange order by ID";
    query.exec(sql);
    int i=0;
    while(query.next())
    {
        rangeSets[i].id = query.value(1).toInt();
        rangeSets[i].active = query.value(2).toInt();

        rangeSets[i].def[0] = query.value(3).toFloat();
        rangeSets[i].minDefault[0] = query.value(4).toFloat();
        rangeSets[i].min[0] = query.value(5).toFloat();
        rangeSets[i].maxDefault[0] = query.value(6).toFloat();
        rangeSets[i].max[0] = query.value(7).toFloat();
        rangeSets[i].precDefault[0] = query.value(8).toInt();
        rangeSets[i].prec[0] = query.value(9).toInt();
        rangeSets[i].unit[0] = query.value(10).toString();

        rangeSets[i].def[1] = query.value(11).toFloat();
        rangeSets[i].minDefault[1] = query.value(12).toFloat();
        rangeSets[i].min[1] = query.value(13).toFloat();
        rangeSets[i].maxDefault[1] = query.value(14).toFloat();
        rangeSets[i].max[1] = query.value(15).toFloat();
        rangeSets[i].precDefault[1] = query.value(16).toInt();
        rangeSets[i].prec[1] = query.value(17).toInt();
        rangeSets[i].unit[1] = query.value(18).toString();

        rangeSets[i].def[2] = query.value(19).toFloat();
        rangeSets[i].minDefault[2] = query.value(20).toFloat();
        rangeSets[i].min[2] = query.value(21).toFloat();
        rangeSets[i].maxDefault[2] = query.value(22).toFloat();
        rangeSets[i].max[2] = query.value(23).toFloat();
        rangeSets[i].precDefault[2] = query.value(24).toInt();
        rangeSets[i].prec[2] = query.value(25).toInt();
        rangeSets[i].unit[2] = query.value(26).toString();

        rangeSets[i].def[3] = query.value(27).toFloat();
        rangeSets[i].minDefault[3] = query.value(28).toFloat();
        rangeSets[i].min[3] = query.value(29).toFloat();
        rangeSets[i].maxDefault[3] = query.value(30).toFloat();
        rangeSets[i].max[3] = query.value(31).toFloat();
        rangeSets[i].precDefault[3] = query.value(32).toInt();
        rangeSets[i].prec[3] = query.value(33).toInt();
        rangeSets[i].unit[3] = query.value(34).toString();
        i++;
    }
    db.close();

    itemUnits[0]<<tr("电压")+QString("(%1)").arg(rangeSets[0].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[0].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[0].unit[2]);
    itemUnits[1]<<tr("电压")+QString("(%1)").arg(rangeSets[1].unit[0])<<tr("电阻")+QString("(%1)").arg(rangeSets[1].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[1].unit[2]);
    itemUnits[2]<<tr("电压")+QString("(%1)").arg(rangeSets[2].unit[0])<<tr("电阻")+QString("(%1)").arg(rangeSets[2].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[2].unit[2]);
    itemUnits[3]<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[0])<<tr("电阻")+QString("(%1)").arg(rangeSets[3].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
    itemUnits[4]<<tr("电压")+QString("(%1)").arg(rangeSets[4].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[4].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[4].unit[2]);
    itemUnits[5]<<tr("电压")+QString("(%1)").arg(rangeSets[5].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[5].unit[3])<<tr("功率")+QString("(%1)").arg(rangeSets[5].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[5].unit[2]);
    itemUnits[6]<<tr("电压")+QString("(%1)").arg(rangeSets[6].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[6].unit[1])<<tr("功率")+QString("(%1)").arg(rangeSets[6].unit[3])<<tr("时间")+QString("(%1)").arg(rangeSets[6].unit[2]);
    itemUnits[7]<<tr("电压")+QString("(%1)").arg(rangeSets[7].unit[0])<<tr("电阻")+QString("(%1)").arg(rangeSets[7].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[7].unit[2]);
}

/*************************************
 * 函数名：     saveUser
 * 说明：
*************************************/
void parameter::saveUser()
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

    query.exec(QString("select count(*) from UserList where  User='%1';").arg(name));
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
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
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
 * 说明：      修改密码
*************************************/
void parameter::setUserPassword()
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
void parameter::addUser()
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
void parameter::confirmPassword()
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
void parameter::savePassword()
{
    if(!ui->lineEdit_12->hasFocus())
        return;
    saveUser();
}

/*************************************
 * 函数名：     deleteUser
 * 说明：
*************************************/
void parameter::deleteUser()
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

/*************************************
 * 函数名：readRangeFromDB
 * 说明：
*************************************/
void parameter::readRangeFromDB()
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
        rangeSets[i].id = q.value(1).toInt();
        rangeSets[i].active = q.value(2).toInt();

        rangeSets[i].def[0] = q.value(3).toFloat();
        rangeSets[i].minDefault[0] = q.value(4).toFloat();
        rangeSets[i].min[0] = q.value(5).toFloat();
        rangeSets[i].maxDefault[0] = q.value(6).toFloat();
        rangeSets[i].max[0] = q.value(7).toFloat();
        rangeSets[i].precDefault[0] = q.value(8).toInt();
        rangeSets[i].prec[0] = q.value(9).toInt();
        rangeSets[i].unit[0] = q.value(10).toString();

        rangeSets[i].def[1] = q.value(11).toFloat();
        rangeSets[i].minDefault[1] = q.value(12).toFloat();
        rangeSets[i].min[1] = q.value(13).toFloat();
        rangeSets[i].maxDefault[1] = q.value(14).toFloat();
        rangeSets[i].max[1] = q.value(15).toFloat();
        rangeSets[i].precDefault[1] = q.value(16).toInt();
        rangeSets[i].prec[1] = q.value(17).toInt();
        rangeSets[i].unit[1] = q.value(18).toString();

        rangeSets[i].def[2] = q.value(19).toFloat();
        rangeSets[i].minDefault[2] = q.value(20).toFloat();
        rangeSets[i].min[2] = q.value(21).toFloat();
        rangeSets[i].maxDefault[2] = q.value(22).toFloat();
        rangeSets[i].max[2] = q.value(23).toFloat();
        rangeSets[i].precDefault[2] = q.value(24).toInt();
        rangeSets[i].prec[2] = q.value(25).toInt();
        rangeSets[i].unit[2] = q.value(26).toString();

        rangeSets[i].def[3] = q.value(27).toFloat();
        rangeSets[i].minDefault[3] = q.value(28).toFloat();
        rangeSets[i].min[3] = q.value(29).toFloat();
        rangeSets[i].maxDefault[3] = q.value(30).toFloat();
        rangeSets[i].max[3] = q.value(31).toFloat();
        rangeSets[i].precDefault[3] = q.value(32).toInt();
        rangeSets[i].prec[3] = q.value(33).toInt();
        rangeSets[i].unit[3] = q.value(34).toString();
        i++;
    }
    db.close();

    itemUnits[0]<<tr("电压")+QString("(%1)").arg(rangeSets[0].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[0].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[0].unit[2]);
    itemUnits[1]<<tr("电压")+QString("(%1)").arg(rangeSets[1].unit[0])<<tr("电阻")+QString("(%1)").arg(rangeSets[1].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[1].unit[2]);
    itemUnits[2]<<tr("电压")+QString("(%1)").arg(rangeSets[2].unit[0])<<tr("电阻")+QString("(%1)").arg(rangeSets[2].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[2].unit[2]);
    itemUnits[3]<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电阻")+QString("(%1)").arg(rangeSets[3].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
    itemUnits[4]<<tr("电压")+QString("(%1)").arg(rangeSets[4].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[4].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[4].unit[2]);
    itemUnits[5]<<tr("电压")+QString("(%1)").arg(rangeSets[5].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[5].unit[3])<<tr("功率")+QString("(%1)").arg(rangeSets[5].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[5].unit[2]);
    itemUnits[6]<<tr("电压")+QString("(%1)").arg(rangeSets[6].unit[0])<<tr("电流")+QString("(%1)").arg(rangeSets[6].unit[1])<<tr("功率")+QString("(%1)").arg(rangeSets[6].unit[3])<<tr("时间")+QString("(%1)").arg(rangeSets[6].unit[2]);
    itemUnits[7]<<tr("电压")+QString("(%1)").arg(rangeSets[7].unit[0])<<tr("电阻")+QString("(%1)").arg(rangeSets[7].unit[1])<<""<<tr("时间")+QString("(%1)").arg(rangeSets[7].unit[2]);
}

/*************************************
 * 函数名：     showRange
 * 说明：
*************************************/
void parameter::showRange()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);
    QStringList list;

    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
    {
            if(rangeSets[i].active == 1 && j == 0)
                ui->tableWidget_5->item(i*3,0)->setCheckState(Qt::Checked);
            else if(rangeSets[i].active == 0 && j == 0)
                ui->tableWidget_5->item(i*3,0)->setCheckState(Qt::Unchecked);

            ui->tableWidget_5->item(i*3+j,2)->setText(QString::number(rangeSets[i].def[j],10,rangeSets[i].prec[j]));
            ui->tableWidget_5->item(i*3+j,3)->setText(QString::number(rangeSets[i].minDefault[j],10,rangeSets[i].prec[j]));
            ui->tableWidget_5->item(i*3+j,4)->setText(QString::number(rangeSets[i].min[j],10,rangeSets[i].prec[j]));
            ui->tableWidget_5->item(i*3+j,5)->setText(QString::number(rangeSets[i].maxDefault[j],10,rangeSets[i].prec[j]));
            ui->tableWidget_5->item(i*3+j,6)->setText(QString::number(rangeSets[i].max[j],10,rangeSets[i].prec[j]));
            ui->tableWidget_5->item(i*3+j,7)->setText(QString::number(rangeSets[i].precDefault[j],10));
            ui->tableWidget_5->item(i*3+j,8)->setText(QString::number(rangeSets[i].prec[j],10));

            list.clear();
            if(j == 0)
            {
                list<<"KV"<<"V"<<"mV";
            }else if(j == 2){
                list<<"S"<<"mS";
            }else if(j == 1 && (i == 0 || i == 1 || i == 4)){
                list<<"A"<<"mA"<<"μA";
            }else if(j == 1 && i == 2){
                list<<"MΩ"<<"KΩ"<<"Ω"<<"mΩ";
            }

            QComboBox *it8 = new QComboBox();
            it8->setFont(font);
            it8->insertItems(0,list);
            it8->setCurrentText(rangeSets[i].unit[j]);
            ui->tableWidget_5->setCellWidget(i*3+j,9,(QWidget*)it8);
    }

    if(rangeSets[3].active == 1)
        ui->tableWidget_5->item(9,0)->setCheckState(Qt::Checked);
    else
        ui->tableWidget_5->item(9,0)->setCheckState(Qt::Unchecked);

    for(int j=0;j<4;j++)
    {
        list.clear();
        int t = 0;
        if(j == 0)
        {
            t = 3;
            list<<"A"<<"mA"<<"μA";
        }else if(j == 1){
            t = 0;
            list<<"KV"<<"V"<<"mV";
        }else if(j == 2){
            t = 1;
            list<<"MΩ"<<"KΩ"<<"Ω"<<"mΩ";
        }else if(j == 3){
            t = 2;
            list<<"S"<<"mS";
        }

        QComboBox *it8 = new QComboBox();
        it8->setFont(font);
        it8->insertItems(0,list);
        it8->setCurrentText(rangeSets[3].unit[t]);

        ui->tableWidget_5->item(9+j,2)->setText(QString::number(rangeSets[3].def[t],10,rangeSets[3].prec[t]));
        ui->tableWidget_5->item(9+j,3)->setText(QString::number(rangeSets[3].minDefault[t],10,rangeSets[3].prec[t]));
        ui->tableWidget_5->item(9+j,4)->setText(QString::number(rangeSets[3].min[t],10,rangeSets[3].prec[t]));
        ui->tableWidget_5->item(9+j,5)->setText(QString::number(rangeSets[3].maxDefault[t],10,rangeSets[3].prec[t]));
        ui->tableWidget_5->item(9+j,6)->setText(QString::number(rangeSets[3].max[t],10,rangeSets[3].prec[t]));
        ui->tableWidget_5->item(9+j,7)->setText(QString::number(rangeSets[3].precDefault[t],10));
        ui->tableWidget_5->item(9+j,8)->setText(QString::number(rangeSets[3].prec[t],10));

        ui->tableWidget_5->setCellWidget(9+j,9,(QWidget*)it8);
    }

    if(rangeSets[4].active == 1)
        ui->tableWidget_5->item(13,0)->setCheckState(Qt::Checked);
    else
        ui->tableWidget_5->item(13,0)->setCheckState(Qt::Unchecked);
    for(int j=0;j<3;j++)
    {
        ui->tableWidget_5->item(13+j,2)->setText(QString::number(rangeSets[4].def[j],10,rangeSets[4].prec[j]));
        ui->tableWidget_5->item(13+j,3)->setText(QString::number(rangeSets[4].minDefault[j],10,rangeSets[4].prec[j]));
        ui->tableWidget_5->item(13+j,4)->setText(QString::number(rangeSets[4].min[j],10,rangeSets[4].prec[j]));
        ui->tableWidget_5->item(13+j,5)->setText(QString::number(rangeSets[4].maxDefault[j],10,rangeSets[4].prec[j]));
        ui->tableWidget_5->item(13+j,6)->setText(QString::number(rangeSets[4].max[j],10,rangeSets[4].prec[j]));
        ui->tableWidget_5->item(13+j,7)->setText(QString::number(rangeSets[4].precDefault[j],10));
        ui->tableWidget_5->item(13+j,8)->setText(QString::number(rangeSets[4].prec[j],10));

        list.clear();
        if(j == 0)
        {
            list<<"KV"<<"V"<<"mV";
        }else if(j == 1){
            list<<"A"<<"mA"<<"μA";
        }else if(j == 2){
            list<<"S"<<"mS";
        }

        QComboBox *it8 = new QComboBox();
        it8->setFont(font);
        it8->insertItems(0,list);
        it8->setCurrentText(rangeSets[4].unit[j]);
        ui->tableWidget_5->setCellWidget(13+j,9,(QWidget*)it8);
    }

    for(int i=5;i<7;i++)
        for(int j=0;j<4;j++)
        {
            if(rangeSets[i].active == 1 && j == 0)
                ui->tableWidget_5->item(i*3+(i==6?2:1),0)->setCheckState(Qt::Checked);
            else if(rangeSets[i].active == 0 && j == 0)
                ui->tableWidget_5->item(i*3+(i==6?2:1),0)->setCheckState(Qt::Unchecked);

            list.clear();
            if(j == 0)
            {
                list<<"KV"<<"V"<<"mV";
            }else if(j == 1){
                list<<"A"<<"mA"<<"μA";
            }else if(j == 2){
                list<<"KW"<<"W";
            }else if(j == 3){
                list<<"S"<<"mS";
            }

            QComboBox *it8 = new QComboBox();
            it8->setFont(font);
            it8->insertItems(0,list);
            int t = 0;
            if(j == 0)
            {
                t = 0;
            }else if(j == 1){
                if(i == 5)
                    t = 3;
                else
                    t = 1;
            }else if(j == 2){
                if(i == 5)
                    t = 1;
                else
                    t = 3;
            }else if(j == 3)
                t = 2;

            it8->setCurrentText(rangeSets[i].unit[t]);

            ui->tableWidget_5->item(i*3+j+(i==6?2:1),2)->setText(QString::number(rangeSets[i].def[t],10,rangeSets[i].prec[t]));
            ui->tableWidget_5->item(i*3+j+(i==6?2:1),3)->setText(QString::number(rangeSets[i].minDefault[t],10,rangeSets[i].prec[t]));
            ui->tableWidget_5->item(i*3+j+(i==6?2:1),4)->setText(QString::number(rangeSets[i].min[t],10,rangeSets[i].prec[t]));
            ui->tableWidget_5->item(i*3+j+(i==6?2:1),5)->setText(QString::number(rangeSets[i].maxDefault[t],10,rangeSets[i].prec[t]));
            ui->tableWidget_5->item(i*3+j+(i==6?2:1),6)->setText(QString::number(rangeSets[i].max[t],10,rangeSets[i].prec[t]));
            ui->tableWidget_5->item(i*3+j+(i==6?2:1),7)->setText(QString::number(rangeSets[i].precDefault[t],10));
            ui->tableWidget_5->item(i*3+j+(i==6?2:1),8)->setText(QString::number(rangeSets[i].prec[t],10));

            ui->tableWidget_5->setCellWidget(i*3+j+(i==6?2:1),9,(QWidget*)it8);
        }

    if(rangeSets[7].active == 1)
        ui->tableWidget_5->item(24,0)->setCheckState(Qt::Checked);
    else if(rangeSets[7].active == 0)
        ui->tableWidget_5->item(24,0)->setCheckState(Qt::Unchecked);

    for(int j=0;j<3;j++)
    {
        ui->tableWidget_5->item(24+j,2)->setText(QString::number(rangeSets[7].def[j],10,rangeSets[7].prec[j]));
        ui->tableWidget_5->item(24+j,3)->setText(QString::number(rangeSets[7].minDefault[j],10,rangeSets[7].prec[j]));
        ui->tableWidget_5->item(24+j,4)->setText(QString::number(rangeSets[7].min[j],10,rangeSets[7].prec[j]));
        ui->tableWidget_5->item(24+j,5)->setText(QString::number(rangeSets[7].maxDefault[j],10,rangeSets[7].prec[j]));
        ui->tableWidget_5->item(24+j,6)->setText(QString::number(rangeSets[7].max[j],10,rangeSets[7].prec[j]));
        ui->tableWidget_5->item(24+j,7)->setText(QString::number(rangeSets[7].precDefault[j],10));
        ui->tableWidget_5->item(24+j,8)->setText(QString::number(rangeSets[7].prec[j],10));

        list.clear();
        if(j == 0)
        {
            list<<"KV"<<"V"<<"mV";
        }else if(j == 1){
            list<<"MΩ"<<"KΩ"<<"Ω"<<"mΩ";
        }else if(j == 2){
            list<<"S"<<"mS";
        }

        QComboBox *it8 = new QComboBox();
        it8->setFont(font);
        it8->insertItems(0,list);
        it8->setCurrentText(rangeSets[7].unit[j]);
        ui->tableWidget_5->setCellWidget(24+j,9,(QWidget*)it8);
    }
}

/*************************************
 * 函数名：     saveSenior
 * 说明：
*************************************/
void parameter::saveSenior()
{
    if(ui->radioButton->isChecked())
        tc.language = 0;
    else if(ui->radioButton_2->isChecked())
        tc.language = 1;
    else if(ui->radioButton_5->isChecked())
        tc.language = 2;

    tc.machineModel = ui->comboBox_12->currentIndex();
    tc.phase = ui->comboBox_6->currentIndex();
    tc.itemNumMax = ui->spinBox_8->value();

    if(ui->checkBox_3->checkState() == Qt::Checked)
        tc.md = 1;
    else
        tc.md = 0;

    tc.auxiliary1 = ui->lineEdit_4->text();
    tc.auxiliary2 = ui->lineEdit_14->text();
    tc.auxiliary3 = ui->lineEdit_15->text();
    tc.ACW_I_K = ui->comboBox_18->currentText();
    qDebug()<<"parameter::saveSenior";
}

/*************************************
 * 函数名：     saveParameter
 * 说明：
*************************************/
void parameter::saveParameter()
{
    QString port = ui->comboBox_2->currentText();
    QString sPort = ui->comboBox_3->currentText();
    QString cPort = ui->comboBox_11->currentText();
    if(ui->checkBox->isChecked())
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

    if(ui->checkBox_16->isChecked())
        tc.BI.lengthCheck = 1;
    else
        tc.BI.lengthCheck = 0;
    tc.BI.barCodeLength = ui->spinBox_9->value();

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

    if(ui->checkBox_17->isChecked())
        tc.STD.repeat = 1;
    else
        tc.STD.repeat = 0;
    tc.STD.repeatType = ui->comboBox_17->currentIndex();

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
    tc.PS.PrintType = ui->comboBox_19->currentIndex();
    QString PrintPort = ui->comboBox_20->currentText();
    tc.PS.PrintPort = PrintPort.mid(3,PrintPort.length()-3).toInt();
    tc.PS.PrintBaudRate = ui->comboBox_21->currentText().toInt();

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


    if(tc.DS.ip != ui->lineEdit_3->text())
    {
        tc.DS.ip = ui->lineEdit_3->text();
        emit sendIpRemote();
    }

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

    if(ui->checkBox_15->isChecked())
    {
        tc.SPS.psEnable = 1;
    }else{
        tc.SPS.psEnable = 0;
    }

    QString psPort1 = ui->comboBox_14->currentText();
    QString psPort2 = ui->comboBox_15->currentText();

    tc.SPS.psPort1 = psPort1.mid(3,psPort1.length()-3).toInt();
    tc.SPS.psPort2 = psPort2.mid(3,psPort2.length()-3).toInt();
    tc.SPS.psBaudRate1 = ui->comboBox_13->currentText().toInt();
    tc.SPS.psBaudRate2 = ui->comboBox_16->currentText().toInt();
    tc.SPS.psCoefficient1 = ui->doubleSpinBox_9->value();
    tc.SPS.psCoefficient2 = ui->doubleSpinBox_10->value();
    tc.SPS.psCoefficient3 = ui->doubleSpinBox_11->value();
    tc.SPS.psCoefficient4 = ui->doubleSpinBox_12->value();
    tc.SPS.psMin = ui->doubleSpinBox_13->value();
    tc.SPS.psMax = ui->doubleSpinBox_14->value();
}

/*************************************
 * 函数名：     setLength
 * 说明：
*************************************/
void parameter::setLength(QString t)
{
    ui->spinBox_4->setValue(t.length());
}

void parameter::chooseFile(QModelIndex index)
{
    if(ui->tableWidget_2->item(index.row(),3)->checkState() == Qt::Unchecked)
        ui->tableWidget_2->item(index.row(),3)->setCheckState(Qt::Checked);
    else
        ui->tableWidget_2->item(index.row(),3)->setCheckState(Qt::Unchecked);
}

void parameter::uploadSet()
{
    if(ui->checkBox_10->isChecked())
    {
        ui->comboBox_7->setVisible(true);

        if(ui->comboBox_7->currentIndex() == 3)
        {
            ui->comboBox_17->setVisible(true);
            ui->checkBox_17->setVisible(true);
        }
    }else{
        ui->comboBox_7->setVisible(false);
        ui->comboBox_17->setVisible(false);
        ui->checkBox_17->setVisible(false);
    }
}

void parameter::repeatSet(int index)
{
    if(index == 3)
    {
        ui->comboBox_17->setVisible(true);
        ui->checkBox_17->setVisible(true);
    }else{
        ui->comboBox_17->setVisible(false);
        ui->checkBox_17->setVisible(false);
    }
}

void parameter::txtFilePath()
{
    QString txt_path = QFileDialog::getExistingDirectory(this,tr("TXT文件目录"),".");
    ui->lineEdit_16->setText(txt_path);
}

void parameter::getSerialPortsByCS()
{
    QString fn = QCoreApplication::applicationDirPath()+"/ExcelPanasonic.dll";
    if(QFile::exists(fn))
    {
        typedef QString (*Fun)(void);
        QLibrary mylib("ExcelPanasonic.dll");
        QString result;

        if(mylib.load())
        {
            Fun open = (Fun)mylib.resolve("ConnectionToServer");
            if(open)
            {
                result = open();
                if(result.size() >= 0)
                {
                    qDebug()<<result;
                }
            }else{
                QString error = mylib.errorString();
                qDebug()<<error;
            }
        }
    }
}

void parameter::inicomm()
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
    ui->comboBox_14->clear();
    ui->comboBox_15->clear();
    ui->comboBox_20->clear();

    for(int i=0; i<key.size(); i++)
    {
        comlist << getcomm(i, "value");
    }
    ui->comboBox_2->addItems(comlist);
    ui->comboBox_3->addItems(comlist);
    ui->comboBox_9->addItems(comlist);
    ui->comboBox_11->addItems(comlist);
    ui->comboBox_14->addItems(comlist);
    ui->comboBox_15->addItems(comlist);
    ui->comboBox_20->addItems(comlist);

    qDebug()<<key;
    qDebug()<<comlist;
    delete settings;
}

QString parameter::getcomm(int index,QString keyorvalue)
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

/*************************************
 * 函数名：     readParameter
 * 说明：
*************************************/
void parameter::readParameter(int index)
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    switch(index)
    {
        case 0:
        {
            disconnect(ui->toolButton_15,SIGNAL(clicked(bool)),this,SLOT(saveParameter()));
            disconnect(ui->spinBox_2,SIGNAL(valueChanged(QString)),this,SLOT(setLength(QString)));
            disconnect(ui->toolButton_7,SIGNAL(clicked(bool)),this,SLOT(openClosePort()));
            disconnect(ui->toolButton_3,SIGNAL(clicked(bool)),this,SLOT(openCloseSPort()));
            disconnect(ui->toolButton_4,SIGNAL(clicked(bool)),this,SLOT(txtFilePath()));            
            disconnect(ui->checkBox_10,SIGNAL(stateChanged(int)),this,SLOT(uploadSet()));
            disconnect(ui->comboBox_7,SIGNAL(currentIndexChanged(int)),this,SLOT(repeatSet(int)));
//            getSerialPortsByCS();

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

            if(tc.BI.lengthCheck == 1)
            {
                ui->checkBox_16->setCheckState(Qt::Checked);
            }else{
                ui->checkBox_16->setCheckState(Qt::Unchecked);
            }
            ui->spinBox_9->setValue(tc.BI.barCodeLength);

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
                if(ui->comboBox_7->currentIndex() == 3)
                {
                    ui->comboBox_17->setVisible(true);
                    ui->checkBox_17->setVisible(true);
                }else{
                    ui->comboBox_17->setVisible(false);
                    ui->checkBox_17->setVisible(false);
                }
            }else{
                ui->checkBox_10->setCheckState(Qt::Unchecked);
                ui->comboBox_7->setVisible(false);
                ui->comboBox_17->setVisible(false);
                ui->checkBox_17->setVisible(false);
                ui->comboBox_7->setCurrentIndex(tc.STD.uploadType);
            }

            if(tc.STD.repeat == 1)
            {
                ui->checkBox_17->setCheckState(Qt::Checked);
//                ui->comboBox_17->setVisible(true);
                ui->comboBox_17->setCurrentIndex(tc.STD.repeatType);
            }else{
                ui->checkBox_17->setCheckState(Qt::Unchecked);                
                ui->comboBox_17->setCurrentIndex(tc.STD.repeatType);
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
            ui->comboBox_19->setCurrentIndex(tc.PS.PrintType);

            ui->label_32->setVisible(false);

            inicomm();
            ui->comboBox_2->setCurrentText(QString("COM%1").arg(tc.SPS.port));
            ui->comboBox_3->setCurrentText(QString("COM%1").arg(tc.SPS.sPort));
            ui->comboBox->setCurrentText(QString("%1").arg(tc.SPS.baudRate));
            ui->comboBox_4->setCurrentText(QString("%1").arg(tc.SPS.sBaudRate));
            ui->spinBox_5->setValue(tc.SPS.machineAddr);
            ui->comboBox_9->setCurrentText(QString("COM%1").arg(tc.SPS.pplPort));
            ui->comboBox_8->setCurrentText(QString("%1").arg(tc.SPS.pplBaudRate));
            ui->comboBox_11->setCurrentText(QString("COM%1").arg(tc.SPS.cPort));
            ui->comboBox_10->setCurrentText(QString("%1").arg(tc.SPS.cBaudRate));
            ui->comboBox_20->setCurrentText(QString("COM%1").arg(tc.PS.PrintPort));
            ui->comboBox_21->setCurrentText(QString("%1").arg(tc.PS.PrintBaudRate));

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

            if(tc.SPS.psEnable == 1)
                ui->checkBox_15->setCheckState(Qt::Checked);
            else
                ui->checkBox_15->setCheckState(Qt::Unchecked);

            ui->comboBox_14->setCurrentText(QString("COM%1").arg(tc.SPS.psPort1));
            ui->comboBox_15->setCurrentText(QString("COM%1").arg(tc.SPS.psPort2));
            ui->comboBox_13->setCurrentText(QString("%1").arg(tc.SPS.psBaudRate1));
            ui->comboBox_16->setCurrentText(QString("%1").arg(tc.SPS.psBaudRate2));
            ui->doubleSpinBox_9->setValue(tc.SPS.psCoefficient1);
            ui->doubleSpinBox_10->setValue(tc.SPS.psCoefficient2);
            ui->doubleSpinBox_11->setValue(tc.SPS.psCoefficient3);
            ui->doubleSpinBox_12->setValue(tc.SPS.psCoefficient4);
            ui->doubleSpinBox_13->setValue(tc.SPS.psMin);
            ui->doubleSpinBox_14->setValue(tc.SPS.psMax);
            ui->comboBox_20->setCurrentText(QString("COM%1").arg(tc.PS.PrintPort));
            ui->comboBox_21->setCurrentText(QString("%1").arg(tc.PS.PrintBaudRate));

            connect(ui->toolButton_4,SIGNAL(clicked(bool)),this,SLOT(txtFilePath()));
            connect(ui->toolButton_3,SIGNAL(clicked(bool)),this,SLOT(openCloseSPort()));
            connect(ui->toolButton_7,SIGNAL(clicked(bool)),this,SLOT(openClosePort()));
            connect(ui->toolButton_15,SIGNAL(clicked(bool)),this,SLOT(saveParameter()));
            connect(ui->spinBox_2,SIGNAL(valueChanged(QString)),this,SLOT(setLength(QString)));
            connect(ui->checkBox_10,SIGNAL(stateChanged(int)),this,SLOT(uploadSet()));
            connect(ui->comboBox_7,SIGNAL(currentIndexChanged(int)),this,SLOT(repeatSet(int)));
        }
            break;
        case 1:
        {
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
            break;
        case 2:
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
            break;
        case 3:
        {
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
            break;
        case 4:
        {
            disconnect(ui->toolButton_13,SIGNAL(clicked(bool)),this,SLOT(saveRange()));
            disconnect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            disconnect(ui->tableWidget_5,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(checkItem(QTableWidgetItem*)));
            ui->tableWidget_5->item(0, 0)->setFlags(ui->tableWidget_5->item(0, 0)->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_5->item(3, 0)->setFlags(ui->tableWidget_5->item(3, 0)->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_5->item(6, 0)->setFlags(ui->tableWidget_5->item(6, 0)->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_5->item(9, 0)->setFlags(ui->tableWidget_5->item(9, 0)->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_5->item(13, 0)->setFlags(ui->tableWidget_5->item(13, 0)->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_5->item(16, 0)->setFlags(ui->tableWidget_5->item(16, 0)->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_5->item(20, 0)->setFlags(ui->tableWidget_5->item(20, 0)->flags() & ~Qt::ItemIsEditable);
            ui->tableWidget_5->item(24, 0)->setFlags(ui->tableWidget_5->item(20, 0)->flags() & ~Qt::ItemIsEditable);
            changed = 0;

            for(int i=0;i<25;i++)
                for(int j=2;j<9;j++)
                {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setTextAlignment(Qt::AlignCenter);
                    ui->tableWidget_5->setItem(i,j,item);

                }
            showRange();
            connect(ui->toolButton_13,SIGNAL(clicked(bool)),this,SLOT(saveRange()));
            connect(ui->tableWidget_5,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            connect(ui->tableWidget_5,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(checkItem(QTableWidgetItem*)));
        }
            break;
        case 5:
        {
            disconnect(ui->toolButton_14,SIGNAL(clicked(bool)),this,SLOT(saveSenior()));
            if(tc.language == 0)
            {
                ui->radioButton->setChecked(true);
                ui->radioButton_2->setChecked(false);
                ui->radioButton_5->setChecked(false);
            }else if(tc.language == 1){
                ui->radioButton->setChecked(false);
                ui->radioButton_2->setChecked(true);
                ui->radioButton_5->setChecked(false);
            }else if(tc.language == 2){
                ui->radioButton->setChecked(false);
                ui->radioButton_2->setChecked(false);
                ui->radioButton_5->setChecked(true);
            }

            ui->comboBox_12->setCurrentIndex(tc.machineModel);
            ui->comboBox_6->setCurrentIndex(tc.phase);
            ui->comboBox_18->setCurrentText(tc.ACW_I_K);

            if(tc.md == 1)
                ui->checkBox_3->setCheckState(Qt::Checked);
            else
                ui->checkBox_3->setCheckState(Qt::Unchecked);

            ui->spinBox_8->setValue(tc.itemNumMax);
            ui->lineEdit_4->setText(tc.auxiliary1);
            ui->lineEdit_14->setText(tc.auxiliary2);
            ui->lineEdit_15->setText(tc.auxiliary3);
            connect(ui->toolButton_14,SIGNAL(clicked(bool)),this,SLOT(saveSenior()));
        }
            break;
    }
}

/*************************************
 * 函数名：     writeIniFile
 * 说明：
*************************************/
void parameter::writeIniFile()
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

    configIniWrite->setValue("set/lastGroup",tc.lastGroup);
    configIniWrite->setValue("set/phase",QString("%1").arg(tc.phase));
    configIniWrite->setValue("set/md",QString("%1").arg(tc.md));
    configIniWrite->setValue("set/testNum",QString("%1").arg(tc.total));
    configIniWrite->setValue("set/passNum",QString("%1").arg(tc.passNum));
    configIniWrite->setValue("set/codeMode",QString("%1").arg(tc.codeMode));
    configIniWrite->setValue("set/auxiliary1",tc.auxiliary1);
    configIniWrite->setValue("set/auxiliary2",tc.auxiliary2);
    configIniWrite->setValue("set/auxiliary3",tc.auxiliary3);
    configIniWrite->setValue("set/txtEnable",QString("%1").arg(tc.txtEnable));
    configIniWrite->setValue("set/txtFile",tc.txtFile);
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

    configIniWrite->setValue("set/autoPrint",QString("%1").arg(tc.PS.autoPrint));
    configIniWrite->setValue("set/PrintType",QString("%1").arg(tc.PS.PrintType));
    configIniWrite->setValue("set/printFail",QString("%1").arg(tc.PS.printFail));
    configIniWrite->setValue("set/printPass",QString("%1").arg(tc.PS.printPass));
    configIniWrite->setValue("set/PrintPort",QString("%1").arg(tc.PS.PrintPort));
    configIniWrite->setValue("set/PrintBaudRate",QString("%1").arg(tc.PS.PrintBaudRate));

    configIniWrite->setValue("set/ACW_I_K",QString("%1").arg(tc.ACW_I_K));

    delete configIniWrite;
}

/*************************************
 * 函数名：     exitRange
 * 说明：
*************************************/
void parameter::exitParameter()
{
    writeIniFile();
    this->close();
    this->deleteLater();
}

/*************************************
 * 函数名：     openClosePort
 * 说明：
*************************************/
void parameter::openClosePort()
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
        sendCommand(1);
    }else{
        ui->toolButton_7->setText(tr("关闭"));
        sendCommand(0);
    }
}

void parameter::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

/*************************************
 * 函数名：     openCloseSPort
 * 说明：
*************************************/
void parameter::openCloseSPort()
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
    qDebug()<<tc.sPortOpen;
}
