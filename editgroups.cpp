#include "editGroups.h"
#include "ui_editgroups.h"

int activeRow;
editGroups::editGroups(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::editGroups)
{
    ui->setupUi(this);

    saved = 1;
    saveAs = 0;
    activeRow = 0;
    model = new QStandardItemModel(ui->listView);
    ui->listView->setModel(model);

    if(tc.phase == 1 || tc.phase == 2)
    {
        ui->label_3->setVisible(true);
        ui->comboBox->setVisible(true);
    }else{
        ui->label_3->setVisible(false);
        ui->comboBox->setVisible(false);
    }

    if(tc.machineModel == 3)
    {
        ui->label_4->setVisible(true);
        ui->label_5->setVisible(true);
        ui->spinBox_2->setVisible(true);
    }else{
        ui->label_4->setVisible(false);
        ui->label_5->setVisible(false);
        ui->spinBox_2->setVisible(false);
    }

    if(tc.machineModel == 4)
    {
        ui->label_7->setVisible(true);
        ui->comboBox_3->setVisible(true);
    }else{
        ui->label_7->setVisible(false);
        ui->comboBox_3->setVisible(false);
    }

    if(tc.md == 1)
    {
        ui->label_6->setVisible(true);
        ui->comboBox_2->setVisible(true);
    }else{
        ui->label_6->setVisible(false);
        ui->comboBox_2->setVisible(false);
    }
    ui->label_39->setVisible(false);
    ui->spinBox->setVisible(false);
}

editGroups::~editGroups()
{
    delete ui;
}

void editGroups::singAndSlotConn()
{
    connect(ui->actionAdd,SIGNAL(triggered()),this,SLOT(on_add_clicked()));
    connect(ui->actionSave,SIGNAL(triggered()),this,SLOT(on_save_clicked()));
    connect(ui->actionDelete,SIGNAL(triggered()),this,SLOT(on_delete_clicked()));
    connect(ui->actionNew,SIGNAL(triggered()),this,SLOT(on_new_clicked()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(on_open_clicked()));
    connect(ui->actionSaveAs,SIGNAL(triggered()),this,SLOT(on_saveAs_clicked()));
    connect(ui->actionSend,SIGNAL(triggered()),this,SLOT(sendGroup()));
    connect(ui->actionExit,SIGNAL(triggered()),this,SLOT(on_exit_clicked()));

    connect(ui->listView,SIGNAL(clicked(QModelIndex)),this,SLOT(on_clicked_clicked(QModelIndex)));
    connect(ui->spinBox,SIGNAL(valueChanged(int)),this,SLOT(on_edit_clicked()));
    connect(ui->lineEdit,SIGNAL(textChanged(QString)),this,SLOT(on_edit_clicked()));
    connect(ui->lineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(on_edit_clicked()));
    connect(ui->doubleSpinBox_22,SIGNAL(valueChanged(double)),this,SLOT(on_edit_clicked()));
    connect(ui->doubleSpinBox_23,SIGNAL(valueChanged(double)),this,SLOT(on_edit_clicked()));
    connect(ui->comboBox_14,SIGNAL(currentIndexChanged(int)),this,SLOT(on_edit_clicked()));
    connect(ui->comboBox_15,SIGNAL(currentIndexChanged(int)),this,SLOT(on_gnd_clicked(int)));
    connect(ui->comboBox_16,SIGNAL(currentIndexChanged(int)),this,SLOT(on_edit_clicked()));
    connect(ui->comboBox_17,SIGNAL(currentIndexChanged(int)),this,SLOT(on_edit_clicked()));

    if(tc.phase == 1 || tc.phase == 2)
    {
        connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(on_phase_clicked(int)));
    }
}

/****************************************
 * 方法名：closeEvent
 * 说明：
****************************************/
void editGroups::closeEvent(QCloseEvent *event)
{
    if(saved == 0)
    {
        QMessageBox box(QMessageBox::Question,tr("保存测试程序"),
                        tr("是否保存对测试程序所做的修改?"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,QString(tr("保存")));
        box.setButtonText(QMessageBox::Cancel,QString(tr("不保存")));
        int ret = box.exec();
        if(ret == QMessageBox::Ok)
        {
            on_save_clicked();
        }
    }
    this->close();
    this->deleteLater();
}

void editGroups::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents();
    }
}

/****************************************
 * 方法名：sendGroup
 * 说明：
****************************************/
void editGroups::sendGroup()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    saved = 1;
    delay(100);
    int ok = on_save_clicked();
    if(ok == -1)
    {
        QApplication::restoreOverrideCursor();
        return;
    }
    delay(100);
    emit sendGroupFromEdit(groupData.groupNum,groupData.groupName);
    QApplication::restoreOverrideCursor();
    on_exit_clicked();
}

void editGroups::on_exit_clicked()
{
    if(saved == 0)
    {
        QMessageBox box(QMessageBox::Question,tr("保存测试程序"),
                        tr("是否保存对测试程序所做的修改?"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,QString(tr("保存")));
        box.setButtonText(QMessageBox::Cancel,QString(tr("不保存")));
        int ret = box.exec();
        if(ret == QMessageBox::Ok)
        {
            on_save_clicked();
        }
        saved = 1;
    }
    this->close();
    this->deleteLater();
}

/****************************************
 * 方法名：receiveData
 * 说明：
****************************************/
void editGroups::receiveData(QString data)
{
    openGroupName = data;
}

/****************************************
 * 方法名：receiveData
 * 说明：
****************************************/
void editGroups::receiveData(int index)
{
    for(int i=0;i<ui->listView->model()->rowCount();i++)
    {
        if(i == index)
            groupItem_List[i]->setBackgroundColor(1);
        else
            groupItem_List[i]->setBackgroundColor(0);
    }
}

/****************************************
 * 方法名：receiveChange
 * 说明：
****************************************/
void editGroups::receiveChange(int index)
{
    saved = 0;
}

/****************************************
 * 方法名：on_open_clicked
 * 说明：
****************************************/
void editGroups::on_open_clicked()
{
    if(saved == 0)
    {
        QMessageBox box(QMessageBox::Question,tr("保存测试程序"),
                        tr("是否保存对测试程序所做的修改?"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,tr("保存"));
        box.setButtonText(QMessageBox::Cancel,tr("不保存"));
        int ret = box.exec();
        if(ret == QMessageBox::Ok)
        {
            on_save_clicked();
        }
    }

    saved = 1;
    saveAs = 0;
    openType = 0;

    chooseGroup *group;
    group = new chooseGroup();
    group->setWindowFlags(group->windowFlags() | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
    group->setFixedSize(group->width(),group->height());
    group->userLevel = tc.userLevel;
    if(type == 1)
        group->readSelfCheckGroup();
    else
        group->readGroup("");
    group->setFillShow();
    connect(group, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
    openType = 0;
    if(group->exec() == QDialog::Accepted)
    {
        if(groupItem_List.size() > 0)
            groupItem_List.clear();

        if(standardItem_List.size() > 0)
            standardItem_List.clear();

        model->clear();
        if(openType == 0 || openType == 2)
            readGroupFromFile(openGroupName);
        else
            newGroup();
    }
}

/****************************************
 * 方法名：on_edit_clicked
 * 说明：
****************************************/
void editGroups::on_edit_clicked()
{
    saved = 0;
}

/****************************************
 * 方法名：on_phase_clicked
 * 说明：
****************************************/
void editGroups::on_phase_clicked(int index)
{
    saved = 0;
    for(int i=0;i<groupItem_List.count();i++)
    {
		groupItem_List[i]->setItemStateST(index,groupItem_List[i]->itemName->currentText());
    }
}

/****************************************
 * 方法名：on_gnd_clicked
 * 说明：
****************************************/
void editGroups::on_gnd_clicked(int index)
{
    saved = 0;
    for(int i=0;i<groupItem_List.count();i++)
    {
        groupItem_List[i]->setGNDShow(index);
        if(groupItem_List[i]->itemName->currentText() == tr("接地"))
        {
            if(index == 0)
            {
                groupItem_List[i]->setItemUnit(getItemUnits(4,0));
                groupItem_List[i]->setRange(4);
            }else{
                groupItem_List[i]->setItemUnit(getItemUnits(4,1));
                groupItem_List[i]->setRange(4);
            }
        }
    }
}

/****************************************
 * 方法名：on_new_clicked
 * 说明：
****************************************/
void editGroups::on_new_clicked()
{
    newGroup();
}

/****************************************
 * 方法名：on_saveAs_clicked
 * 说明：
****************************************/
void editGroups::on_saveAs_clicked()
{
    if(saved == 0)
    {
        QMessageBox box(QMessageBox::Question,tr("保存测试程序"),
                        tr("是否保存对测试程序所做的修改?"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,tr("保存"));
        box.setButtonText(QMessageBox::Cancel,tr("不保存"));
        int ret = box.exec();
        if(ret == QMessageBox::Ok)
        {
            on_save_clicked();
        }
    }

    saved = 1;
    saveAs = 1;
    openType = 1;
    ui->lineEdit->setReadOnly(false);
    ui->lineEdit->setText("");
    ui->lineEdit->setFocus();
}

/****************************************
 * 方法名：checkGroupName
 * 说明：
****************************************/
bool editGroups::checkGroupName(QString name)
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
        return false;
    }

    QSqlQuery q;
    QString sql = "select count(*) from TestProgram where TPName='"+name+"';";
    q.exec(sql);
    q.next();

    if(q.value(0).toInt() == 0)
        ok = true;
    else
        ok = false;
    q.clear();
    db.close();
    return ok;
}

/****************************************
 * 方法名：on_save_clicked
 * 说明：
****************************************/
int editGroups::on_save_clicked()
{
//    qDebug()<<saved<<groupItem_List.count();

    if(groupItem_List.count() <= 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("保存"),tr("请添加测试项!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return -1;
    }

    if(ui->lineEdit->text() == "")
    {
        QMessageBox box(QMessageBox::Warning,tr("保存"),tr("测试组名不能为空!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        ui->lineEdit->setFocus();
        return -1;
    }

    if(!checkGroupName(ui->lineEdit->text()) && openType == 1)
    {
        QMessageBox box(QMessageBox::Question,tr("保存测试程序"),"已存在名为：\n"+ui->lineEdit->text()+"\n的测试程序，是否继续？");
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,tr("覆 盖"));
        box.setButtonText(QMessageBox::Cancel,tr("取 消"));
        int ret = box.exec();
        if(ret == QMessageBox::Cancel)
        {
            ui->lineEdit->selectAll();
            ui->lineEdit->setFocus();
            return -1;
        }
    }

    int cf = checkIDCode(ui->lineEdit_2->text());

    if(cf == -1)
    {
        QMessageBox box(QMessageBox::Warning,tr("识别码重复"),tr("此识别码已存在!"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        ui->lineEdit_2->setFocus();
        return -1;
    }

    ui->lineEdit->setReadOnly(true);

    groupData.groupNum = ui->spinBox->value();
    groupData.groupName = ui->lineEdit->text();
    groupData.IDCode = ui->lineEdit_2->text();
    groupData.upTime = ui->doubleSpinBox_22->value();
    groupData.downTime = ui->doubleSpinBox_23->value();
    groupData.frequency = ui->comboBox_14->currentIndex();
    groupData.gnd = ui->comboBox_15->currentIndex();
    groupData.NG = ui->comboBox_16->currentIndex();
    groupData.invalid = ui->comboBox_17->currentIndex();
    groupData.phase = ui->comboBox->currentIndex();
    groupData.MD = ui->comboBox_2->currentIndex();
    groupData.ACDC = ui->comboBox_3->currentIndex();
    groupData.hotVoltage = ui->spinBox_2->value();
    groupData.itemNum = groupItem_List.count();
    groupData.saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    for(int i=0;i<groupItem_List.count();i++)
    {
        groupItem_List[i]->itemConfigRead(&groupData.items[i]);
    }

    if(tc.machineModel == 4)
    {
        int ok = 0;
        QList<int> pList;

        for(int i=0;i<groupData.itemNum;i++)
        {
            if(groupData.items[i].id == 6)
            {
                pList<<i;
                ok++;
            }
        }

        if(ok > 0)
        {
            if(pList.last() != groupData.itemNum-1)
            {
                QMessageBox box(QMessageBox::Warning,tr("保存"),tr("功率应放在测试程序最后!"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return -1;
            }

            int ok1 = 0;
            int la = pList.last();
            for(int i=pList.size()-1;i>=0;i--)
            {
//                qDebug()<<"pList"<<pList.at(i)<<la;
                if(pList.at(i) != la)
                    ok1 = -1;
                la--;
            }

            if(ok1 == -1)
            {
                QMessageBox box(QMessageBox::Warning,tr("保存"),tr("功率项应连续设置，功率项之间不要设置其他测试项!"));
                box.setWindowFlags(Qt::WindowStaysOnTopHint);
                box.setStandardButtons(QMessageBox::Ok);
                box.setButtonText(QMessageBox::Ok,tr("确 定"));
                box.exec();
                return -1;
            }
        }
    }

    QString fileName = QCoreApplication::applicationDirPath()+"/files/"+groupData.groupName+".txt";
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text );
    QTextStream in(&file);

    in<<"[BaseInfo]"<<"\n";
    in<<QString("GroupNum = \"%1\"").arg(groupData.groupNum)<<"\n";
    in<<QString("ItemNum = \"%1\"").arg(groupData.itemNum)<<"\n";
    in<<QString("IDCode = \"%1\"").arg(groupData.IDCode)<<"\n";
    in<<QString("UpTime = \"%1\"").arg(QString::number(groupData.upTime,10,1))<<"\n";
    in<<QString("DownTime = \"%1\"").arg(QString::number(groupData.downTime,10,1))<<"\n";
    in<<QString("NG = \"%1\"").arg(groupData.NG)<<"\n";
    in<<QString("invalid = \"%1\"").arg(groupData.invalid)<<"\n";
    in<<QString("Freq = \"%1\"").arg(groupData.frequency)<<"\n";
    in<<QString("GNDType = \"%1\"").arg(groupData.gnd)<<"\n";
    in<<QString("Phase = \"%1\"").arg(groupData.phase)<<"\n";
    in<<QString("MD = \"%1\"").arg(groupData.MD)<<"\n";
    in<<QString("ACDC = \"%1\"").arg(groupData.ACDC)<<"\n";
    in<<QString("hotVoltage = \"%1\"").arg(groupData.hotVoltage)<<"\n";
//    in<<QString("machineModel = \"%1\"").arg(tc.machineModel)<<"\n";
    in<<QString("DateTime = \"%1\"").arg(groupData.saveTime)<<"\n";

    in<<"\n";
    in<<"[ItemList]"<<"\n";

    for(int i=0;i<groupData.itemNum;i++)
    {
        if(groupData.items[i].id == 0)
        {
            in<<QString("item%1 = \"%2,%3\"").arg(i+1).arg(i+1).arg(groupData.items[i].id)<<"\n";
//            break;
        }else{
            if(groupData.items[i].id == 6 && tc.machineModel == 1)
            {
                in<<QString("item%1 = \"%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19,%20,%21,%22,%23,%24,%25\"").arg(i+1)
                    .arg(i+1).arg(groupData.items[i].id)
                    .arg(QString::number(groupData.items[i].data[0],10,groupData.items[i].prec[0]))
                        .arg(groupData.items[i].prec[0]).arg(groupData.items[i].unit[0])
                        .arg(QString::number(groupData.items[i].data[1],10,groupData.items[i].prec[1]))
                        .arg(groupData.items[i].prec[1]).arg(groupData.items[i].unit[1])
                        .arg(QString::number(groupData.items[i].data[2],10,groupData.items[i].prec[2]))
                        .arg(groupData.items[i].prec[2]).arg(groupData.items[i].unit[2])
                        .arg(QString::number(groupData.items[i].data[3],10,groupData.items[i].prec[3]))
                        .arg(groupData.items[i].prec[3]).arg(groupData.items[i].unit[3])
                        .arg(groupData.items[i].state)
                        .arg(QString::number(groupData.items[i].data[4],10,groupData.items[i].prec[4]))
                        .arg(groupData.items[i].prec[4]).arg(groupData.items[i].unit[4])
                        .arg(QString::number(groupData.items[i].data[5],10,groupData.items[i].prec[5]))
                        .arg(groupData.items[i].prec[5]).arg(groupData.items[i].unit[5])
                        .arg(QString::number(groupData.items[i].data[6],10,1))
                        .arg(1).arg("S")<<"\n";
            }else if(groupData.items[i].id == 6 && (tc.machineModel == 2 || tc.machineModel == 5)){
                in<<QString("item%1 = \"%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19\"").arg(i+1)
                    .arg(i+1).arg(groupData.items[i].id)
                    .arg(QString::number(groupData.items[i].data[0],10,groupData.items[i].prec[0]))
                        .arg(groupData.items[i].prec[0]).arg(groupData.items[i].unit[0])
                        .arg(QString::number(groupData.items[i].data[1],10,groupData.items[i].prec[1]))
                        .arg(groupData.items[i].prec[1]).arg(groupData.items[i].unit[1])
                        .arg(QString::number(groupData.items[i].data[2],10,groupData.items[i].prec[2]))
                        .arg(groupData.items[i].prec[2]).arg(groupData.items[i].unit[2])
                        .arg(QString::number(groupData.items[i].data[3],10,groupData.items[i].prec[3]))
                        .arg(groupData.items[i].prec[3]).arg(groupData.items[i].unit[3])
                        .arg(groupData.items[i].state)
                        .arg(QString::number(groupData.items[i].data[6],10,1))
                        .arg(1).arg("S")<<"\n";
            }else if((groupData.items[i].id == 3 || groupData.items[i].id == 7) && tc.machineModel == 1){
                in<<QString("item%1 = \"%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19\"").arg(i+1)
                    .arg(i+1).arg(groupData.items[i].id)
                    .arg(QString::number(groupData.items[i].data[0],10,groupData.items[i].prec[0]))
                        .arg(groupData.items[i].prec[0]).arg(groupData.items[i].unit[0])
                        .arg(QString::number(groupData.items[i].data[1],10,groupData.items[i].prec[1]))
                        .arg(groupData.items[i].prec[1]).arg(groupData.items[i].unit[1])
                        .arg(QString::number(groupData.items[i].data[2],10,groupData.items[i].prec[2]))
                        .arg(groupData.items[i].prec[2]).arg(groupData.items[i].unit[2])
                        .arg(QString::number(groupData.items[i].data[3],10,groupData.items[i].prec[3]))
                        .arg(groupData.items[i].prec[3]).arg(groupData.items[i].unit[3])
                        .arg(groupData.items[i].state)
                        .arg(QString::number(groupData.items[i].data[6],10,1))
                        .arg(1).arg("S")<<"\n";
            }else if((groupData.items[i].id == 1 ||groupData.items[i].id == 2 || groupData.items[i].id == 3) && tc.machineModel == 6){
                in<<QString("item%1 = \"%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16,%17,%18,%19\"").arg(i+1)
                    .arg(i+1).arg(groupData.items[i].id)
                    .arg(QString::number(groupData.items[i].data[0],10,groupData.items[i].prec[0]))
                        .arg(groupData.items[i].prec[0]).arg(groupData.items[i].unit[0])
                        .arg(QString::number(groupData.items[i].data[1],10,groupData.items[i].prec[1]))
                        .arg(groupData.items[i].prec[1]).arg(groupData.items[i].unit[1])
                        .arg(QString::number(groupData.items[i].data[2],10,groupData.items[i].prec[2]))
                        .arg(groupData.items[i].prec[2]).arg(groupData.items[i].unit[2])
                        .arg(QString::number(groupData.items[i].data[3],10,groupData.items[i].prec[3]))
                        .arg(groupData.items[i].prec[3]).arg(groupData.items[i].unit[3])
                        .arg(groupData.items[i].state)
                        .arg(QString::number(groupData.items[i].data[6],10,1))
                        .arg(1).arg("S")<<"\n";
            }else{
                in<<QString("item%1 = \"%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12,%13,%14,%15,%16\"").arg(i+1)
                    .arg(i+1).arg(groupData.items[i].id)
                    .arg(QString::number(groupData.items[i].data[0],10,groupData.items[i].prec[0]))
                        .arg(groupData.items[i].prec[0]).arg(groupData.items[i].unit[0])
                        .arg(QString::number(groupData.items[i].data[1],10,groupData.items[i].prec[1]))
                        .arg(groupData.items[i].prec[1]).arg(groupData.items[i].unit[1])
                        .arg(QString::number(groupData.items[i].data[2],10,groupData.items[i].prec[2]))
                        .arg(groupData.items[i].prec[2]).arg(groupData.items[i].unit[2])
                        .arg(QString::number(groupData.items[i].data[3],10,groupData.items[i].prec[3]))
                        .arg(groupData.items[i].prec[3]).arg(groupData.items[i].unit[3])
                        .arg(groupData.items[i].state)<<"\n";
            }
        }
    }

    file.flush();
    file.close();

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
        QMessageBox messageBox;
        messageBox.setText("Database error");
        messageBox.exec();
        return -1;
    }

    QSqlQuery q;
    QString sql;

    sql = "select count(*) from TestProgram where TPName='"+groupData.groupName+"';";
    q.exec(sql);
    q.next();
    int num = q.value(0).toInt();
    q.clear();

    if(num == 0)
    {
        q.prepare("insert into TestProgram (ID,TPName,TPTime,IDCode) VALUES(:v1,:v2,:v3,:v4);");
        q.bindValue(":v1",groupData.groupNum);
        q.bindValue(":v2",groupData.groupName);
        q.bindValue(":v3",groupData.saveTime);
        q.bindValue(":v4",groupData.IDCode);
    }else{
        q.prepare("update TestProgram set ID=:v1,TPTime=:v2,IDCode=:v3 where TPName=:v4;");
        q.bindValue(":v1",groupData.groupNum);
        q.bindValue(":v2",groupData.saveTime);
        q.bindValue(":v3",groupData.IDCode);
        q.bindValue(":v4",groupData.groupName);
    }

    bool isSucceed = q.exec();
    if(!isSucceed)
    {
        QSqlError se = q.lastError();
        QMessageBox messageBox;
        messageBox.setText(se.text());
        messageBox.exec();
    }
    q.clear();
    db.close();
    saved = 1;
    openType = 0;
    return 0;
}

int editGroups::checkIDCode(QString idCode)
{
    if(idCode == "")
        return 0;

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
        return -2;
    }

    QSqlQuery q;
    QString sql;
    int value;
    QString groupName = ui->lineEdit->text();

    sql = QString("select count(*) from TestProgram where IDCode='%1' and TPName<>'%2'")
            .arg(idCode).arg(groupName);
//    sql = "select count(*) from TestProgram where IDCode='"+idCode+"';";
    q.exec(sql);
    q.next();
    int num = q.value(0).toInt();
    q.clear();

    if(num == 0)
    {
        value = 0;
    }else{
        value = -1;
    }

    db.close();
    return value;
}

void editGroups::setCommOffset(int value)
{
    type = value;
}

/****************************************
 * 方法名：newGroup
 * 说明：
****************************************/
void editGroups::newGroup()
{
    saved = 0;
    saveAs = 0;
    activeRow = 0;
    openType = 1;
    ui->spinBox->setValue(2);
    ui->lineEdit->setReadOnly(false);
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->doubleSpinBox_22->setValue(0.0);
    ui->doubleSpinBox_23->setValue(0.0);
    ui->comboBox_14->setCurrentIndex(0);
    ui->comboBox_15->setCurrentIndex(0);
    ui->comboBox_16->setCurrentIndex(1);
    ui->comboBox_17->setCurrentIndex(1);
    if(type == 1)
    {
        ui->spinBox->setValue(1);
        ui->spinBox->setReadOnly(true);
    }else{
        ui->spinBox->setValue(2);
        ui->spinBox->setReadOnly(false);
    }
    ui->spinBox->setFocus();

    if(groupItem_List.count() > 0)
        groupItem_List.clear();

    if(standardItem_List.count() > 0)
        standardItem_List.clear();
    model->clear();
}

/****************************************
 * 方法名：on_clicked_clicked
 * 说明：
****************************************/
void editGroups::on_clicked_clicked(QModelIndex index)
{
    activeRow = index.row();

    for(int i=0;i<ui->listView->model()->rowCount();i++)
    {
        if(i == activeRow)
            groupItem_List[i]->setBackgroundColor(1);
        else
            groupItem_List[i]->setBackgroundColor(0);
    }
}

/****************************************
 * 方法名：on_add_clicked
 * 说明：
****************************************/
void editGroups::on_add_clicked()
{
    int num = 8;
    if(tc.itemNumMax >= 1 && tc.itemNumMax <= 20)
        num = tc.itemNumMax;

    QString info = tr("测试程序最多可添加8个测试项!");
    if(groupItem_List.count() == num)
    {
        QMessageBox box(QMessageBox::Warning,tr("测试项数量"),info);
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    saved = 0;
    int i;
    if(activeRow <= 0)
    {
        i = 0;
    }else if(activeRow < groupItem_List.count()){
        i = activeRow+1;
    }else{
        i = activeRow;
    }

    int gnd = ui->comboBox_15->currentIndex();    

    groupSetItem gi;
    groupData.items<<gi;

    int row = 0;
    QString name = NULL;
    for(int i=0;i<8;i++)
    {
        if(rangeSets[i].active ==1)
        {
            row = i+1;
            switch(row)
            {
                case 1:
                    name = tr("交耐");
                    break;
                case 2:
                    name = tr("直耐");
                    break;
                case 3:
                    name = tr("绝缘");
                    break;
                case 4:
                    name = tr("接地");
                    break;
                case 5:
                    name = tr("泄漏");
                    break;
                case 6:
                    name = tr("功率");
                    break;
                case 7:
                    name = tr("低启");
                    break;
                case 8:
                    name = tr("开短");
                    break;
            }
            break;
        }
    }

    groupItem_List.insert(i,new groupItem(ui->listView));
    standardItem_List.insert(i,new QStandardItem());

    groupItem_List[i]->setItemName(name);
    groupItem_List[i]->setItemUnit(getItemUnits(row,0));

    groupItem_List[i]->setRowNumber(i);
    groupItem_List[i]->setRange(row);
    groupItem_List[i]->checkData(name);

    if(tc.phase == 1 || tc.phase == 2)
    {
        int sst = ui->comboBox->currentIndex();
        groupItem_List[i]->setItemPhase(sst);
    }
    groupItem_List[i]->singAndSlotConn();

    model->insertRow(i,standardItem_List[i]);
    standardItem_List[i]->setSizeHint(QSize(0,ui->listView->size().height()/9));
    QModelIndex index = model->indexFromItem(standardItem_List[i]);
    ui->listView->setIndexWidget(index,groupItem_List[i]);
    groupItem_List[i]->setItemSize(ui->listView->size().width()-ui->listView->verticalScrollBar()->width(),standardItem_List[i]->sizeHint().height());
    groupItem_List[i]->setGNDShow(gnd);
    activeRow++;
    for(int i=0;i<ui->listView->model()->rowCount();i++)
    {
        if(i == activeRow)
            groupItem_List[i]->setBackgroundColor(1);
        else
            groupItem_List[i]->setBackgroundColor(0);
        groupItem_List[i]->setRowNumber(i);
    }
}

/****************************************
 * 方法名：on_delete_clicked
 * 说明：
****************************************/
void editGroups::on_delete_clicked()
{
    saved = 0;
    groupItem_List.removeAt(activeRow);
    standardItem_List.removeAt(activeRow);
    model->removeRow(activeRow);

    if(activeRow >= 1)
        activeRow--;

    for(int i=0;i<ui->listView->model()->rowCount();i++)
    {
        if(i == activeRow)
            groupItem_List[i]->setBackgroundColor(1);
        else
            groupItem_List[i]->setBackgroundColor(0);
    }
}

/****************************************
 * 方法名：getItemUnits
 * 说明：
****************************************/
QStringList editGroups::getItemUnits(int index,int gnd)
{
    QStringList list;
    switch(index)
    {
        case 1:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[0].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[0].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[0].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[0].unit[2]);
            break;
        case 2:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[1].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[1].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[1].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[1].unit[2]);
            break;
        case 3:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[2].unit[0])<<tr("电阻下限")+QString("(%1)").arg(rangeSets[2].unit[1])<<tr("电阻上限")+QString("(%1)").arg(rangeSets[2].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[2].unit[2]);
            break;
        case 4:
            if(gnd == 0)
            {
                list<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电阻下限")+QString("(%1)").arg(rangeSets[3].unit[1])<<tr("电阻上限")+QString("(%1)").arg(rangeSets[3].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
            }else{
                list<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电压下限")+QString("(%1)").arg(rangeSets[3].unit[0])<<tr("电压上限")+QString("(%1)").arg(rangeSets[3].unit[0])<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
            }
            break;
        case 5:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[4].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[4].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[4].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[4].unit[2]);
            break;
        case 6:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[5].unit[0])<<tr("功率下限")+QString("(%1)").arg(rangeSets[5].unit[1])<<tr("功率上限")+QString("(%1)").arg(rangeSets[5].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[5].unit[2]);
            break;
        case 7:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[6].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[6].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[6].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[6].unit[2]);
            break;
        case 8:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[7].unit[0])<<tr("电阻下限")+QString("(%1)").arg(rangeSets[7].unit[1])<<tr("电阻上限")+QString("(%1)").arg(rangeSets[7].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[7].unit[2]);
            break;
        default:
            list<<tr("电压(V)")<<tr("电流下限(mA)")<<tr("电流上限(mA)")<<tr("时间(S)");
            break;
    }
    return list;
}

/****************************************
 * 方法名：showGroup
 * 说明：
****************************************/
void editGroups::showGroup()
{
    for(int i=0;i<groupData.itemNum;i++)
    {
        groupItem_List.append(new groupItem(ui->listView));
        standardItem_List.append(new QStandardItem());
        QStringList unit,data;

        if(groupData.items[i].itemName == tr("无效"))
        {
            data<<""<<""<<""<<"";
            groupItem_List[i]->setItemName("---");
        }else{
            groupItem_List[i]->setItemName(groupData.items[i].itemName);
        }

        if(groupData.items[i].id > 0)
        {
            groupItem_List[i]->setItemUnit(getItemUnits(groupData.items[i].id,groupData.gnd));
        }else{
            unit<<""<<""<<""<<"";
            groupItem_List[i]->setItemUnit(unit);
        }
        connect(groupItem_List[i], SIGNAL(sendData(int)), this, SLOT(receiveData(int)));
        groupItem_List[i]->setRowNumber(i);
        groupItem_List[i]->setRange(groupData.items[i].id);
        groupItem_List[i]->setTestData(groupData.items[i].data);

        if(tc.phase == 1 || tc.phase == 2)
        {
            groupItem_List[i]->setItemStateST(groupData.phase,groupData.items[i].itemName);
        }
        groupItem_List[i]->setItemState(groupData.items[i].state);
        groupItem_List[i]->singAndSlotConn();
        groupItem_List[i]->setGNDShow(groupData.gnd);
        connect(groupItem_List[i], SIGNAL(sendChange(int)), this, SLOT(receiveChange(int)));

        model->appendRow(standardItem_List[i]);
        standardItem_List[i]->setSizeHint(QSize(0,ui->listView->size().height()/9));
        QModelIndex index = model->indexFromItem(standardItem_List[i]);
        ui->listView->setIndexWidget(index,groupItem_List[i]);
        groupItem_List[i]->setItemSize(ui->listView->size().width()-ui->listView->verticalScrollBar()->width(),standardItem_List[i]->sizeHint().height());
    }

    ui->spinBox->setValue(groupData.groupNum);
    ui->lineEdit_2->setText(groupData.IDCode);
    ui->doubleSpinBox_22->setValue(groupData.upTime);
    ui->doubleSpinBox_23->setValue(groupData.downTime);
    ui->comboBox_14->setCurrentIndex(groupData.frequency);
    ui->comboBox_15->setCurrentIndex(groupData.gnd);
    ui->comboBox_16->setCurrentIndex(groupData.NG);
    ui->comboBox_17->setCurrentIndex(groupData.invalid);
    ui->comboBox->setCurrentIndex(groupData.phase);
    ui->comboBox_2->setCurrentIndex(groupData.MD);
    ui->comboBox_3->setCurrentIndex(groupData.ACDC);
    ui->spinBox_2->setValue(groupData.hotVoltage);    
}

/****************************************
 * 方法名：readGroupFromFile
 * 说明：
****************************************/
void editGroups::readGroupFromFile(QString name)
{
    QString fileName = QCoreApplication::applicationDirPath()+"/files/"+name+".txt";
    if(!QFile::exists(fileName))
        return;
    ui->lineEdit->setText(name);
    groupData.groupName = name;

    QSettings *configIniRead = new QSettings(fileName, QSettings::IniFormat);
    configIniRead->setIniCodec(QTextCodec::codecForName("GBK"));

    configIniRead->beginGroup("BaseInfo");
    groupData.groupNum = configIniRead->value("GroupNum").toInt();
    groupData.itemNum = configIniRead->value("ItemNum").toInt();
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

    for(int i=1;i<groupData.itemNum+1;i++)
    {
        a = "item"+QString::number(i,10);
        b = configIniRead->value(a).toString();
        list = b.split(",");

        index = list.at(0).toInt()-1;
        groupSetItem gi;
        groupData.items<<gi;

        switch (list.at(1).toInt())
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

            groupData.items[index].data[1] = list.at(5).toFloat();
            groupData.items[index].prec[1] = list.at(6).toInt();
            groupData.items[index].unit[1] = list.at(7);

            groupData.items[index].data[2] = list.at(8).toFloat();
            groupData.items[index].prec[2] = list.at(9).toInt();
            groupData.items[index].unit[2] = list.at(10);

            groupData.items[index].data[3] = list.at(11).toFloat();
            groupData.items[index].prec[3] = list.at(12).toInt();
            groupData.items[index].unit[3] = list.at(13);
            groupData.items[index].state = list.at(14).toInt();

            if(list.count() == 24 && list.at(1).toInt() == 6)
            {
                groupData.items[index].data[4] = list.at(15).toFloat();
                groupData.items[index].prec[4] = list.at(16).toInt();
                groupData.items[index].unit[4] = list.at(17);

                groupData.items[index].data[5] = list.at(18).toFloat();
                groupData.items[index].prec[5] = list.at(19).toInt();
                groupData.items[index].unit[5] = list.at(20);

                groupData.items[index].data[6] = list.at(21).toFloat();
                groupData.items[index].prec[6] = list.at(22).toInt();
                groupData.items[index].unit[6] = list.at(23);

            }else if(list.count() == 18 && (list.at(1).toInt() == 1 || list.at(1).toInt() == 2 || list.at(1).toInt() == 3 || list.at(1).toInt() == 6 || list.at(1).toInt() == 7)){
                groupData.items[index].data[4] = 0.0;
                groupData.items[index].prec[4] = 1;
                groupData.items[index].unit[4] = "";

                groupData.items[index].data[5] = 0.0;
                groupData.items[index].prec[5] = 1;
                groupData.items[index].unit[5] = "";

                groupData.items[index].data[6] = list.at(15).toFloat();
                groupData.items[index].prec[6] = list.at(16).toInt();
                groupData.items[index].unit[6] = list.at(17);
            }
        }else if(list.at(1).toInt() == 0){
            groupData.items[index].id = 0;
            groupData.items[index].state = -1;
        }

        list.clear();
    }

    configIniRead->endGroup();
    delete configIniRead;
    showGroup();
}

/*************************************
 * 函数名：setItmeSize
 * 说明：
*************************************/
void editGroups::setItmeSize()
{
    int h = ui->listView->size().height()/9;
    int w = ui->listView->size().width()-ui->listView->verticalScrollBar()->width();

    for(int i=0;i<groupData.itemNum;i++)
    {
        standardItem_List[i]->setSizeHint(QSize(0,h));
        groupItem_List[i]->setItemSize(w,h);
    }
}
