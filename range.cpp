#include "range.h"
#include "ui_range.h"

range::range(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::range)
{
    ui->setupUi(this);
    changed = 0;

    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitRange()));
    connect(ui->toolButton_3,SIGNAL(clicked(bool)),this,SLOT(saveRange()));
}

range::~range()
{
    delete ui;
}

void range::tableInit()
{
    QTableWidgetItem *item[24];
    QTableWidgetItem *item1[14];
    QString name[24];
    QFont font("Microsoft YaHei",16,QFont::Bold);
    QFont font1("Microsoft YaHei",12,QFont::Normal);

    QTableWidgetItem *check = new QTableWidgetItem();
    check->setCheckState(Qt::Unchecked);
    check->setText("交耐");
    check->setFont(font);
    check->setFlags(check->flags() & ~Qt::ItemIsEditable);
    check->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(0,0,check);

    QTableWidgetItem *check1 = new QTableWidgetItem();
    check1->setCheckState(Qt::Unchecked);
    check1->setText("直耐");
    check1->setFont(font);
    check1->setFlags(check1->flags() & ~Qt::ItemIsEditable);
    check1->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(3,0,check1);

    QTableWidgetItem *check2 = new QTableWidgetItem();
    check2->setCheckState(Qt::Unchecked);
    check2->setText("绝缘");
    check2->setFont(font);
    check2->setFlags(check2->flags() & ~Qt::ItemIsEditable);
    check2->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(6,0,check2);

    QTableWidgetItem *check3 = new QTableWidgetItem();
    check3->setCheckState(Qt::Unchecked);
    check3->setText("接地");
    check3->setFont(font);
    check3->setFlags(check3->flags() & ~Qt::ItemIsEditable);
    check3->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(9,0,check3);

    QTableWidgetItem *check4 = new QTableWidgetItem();
    check4->setCheckState(Qt::Unchecked);
    check4->setText("泄漏");
    check4->setFont(font);
    check4->setFlags(check4->flags() & ~Qt::ItemIsEditable);
    check4->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(12,0,check4);

    QTableWidgetItem *check5 = new QTableWidgetItem();
    check5->setCheckState(Qt::Unchecked);
    check5->setText("功率");
    check5->setFont(font);
    check5->setFlags(check5->flags() & ~Qt::ItemIsEditable);
    check5->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(15,0,check5);

    QTableWidgetItem *check6 = new QTableWidgetItem();
    check6->setCheckState(Qt::Unchecked);
    check6->setText("低启");
    check6->setFont(font);
    check6->setFlags(check6->flags() & ~Qt::ItemIsEditable);
    check6->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(18,0,check6);

    for(int i=0;i<19;i=i+3)
    {
        name[i] = "电压";
        name[i+2] = "时间";
    }

    name[1] = "电流";
    name[4] = "电阻";
    name[7] = "电阻";
    name[10] = "电阻";
    name[13] = "电流";
    name[16] = "功率";
    name[19] = "电流";

    for(int i=0;i<24;i++)
    {
        item[i] = new QTableWidgetItem();
        item[i]->setText(name[i]);
        item[i]->setFlags(item[i]->flags() & ~Qt::ItemIsEditable);
        item[i]->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,1,item[i]);
    }

    for(int i=1;i<20;i=i+3)
    {
        item1[i] = new QTableWidgetItem();
        item1[i]->setText("");
        item1[i]->setFlags(item1[i]->flags() & ~Qt::ItemIsEditable);
        item1[i]->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i,0,item1[i]);

        item1[i+1] = new QTableWidgetItem();
        item1[i+1]->setText("");
        item1[i+1]->setFlags(item1[i+1]->flags() & ~Qt::ItemIsEditable);
        item1[i+1]->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i+1,0,item1[i+1]);
    }

    QComboBox *comboxList[21];
    for(int i=0;i<21;i++)
        comboxList[i] = new QComboBox();

    QStringList list,list1,list2,list3,list4;
    list<<"KV"<<"V"<<"mV";
    list1<<"A"<<"mA"<<"μA";
    list2<<"MΩ"<<"kΩ"<<"Ω";
    list3<<"kW"<<"W";
    list4<<"S"<<"mS";

    for(int i=0;i<19;i=i+3)
    {
        comboxList[i]->setFont(font1);
        comboxList[i]->insertItems(0,list);
        ui->tableWidget->setCellWidget(i,9,(QWidget*)comboxList[i]);

        comboxList[i+2]->setFont(font1);
        comboxList[i+2]->insertItems(0,list4);
        ui->tableWidget->setCellWidget(i+2,9,(QWidget*)comboxList[i+2]);
    }

    comboxList[1]->setFont(font1);
    comboxList[1]->insertItems(0,list1);
    ui->tableWidget->setCellWidget(1,9,(QWidget*)comboxList[1]);

    comboxList[4]->setFont(font1);
    comboxList[4]->insertItems(0,list2);
    ui->tableWidget->setCellWidget(4,9,(QWidget*)comboxList[4]);

    comboxList[7]->setFont(font1);
    comboxList[7]->insertItems(0,list2);
    ui->tableWidget->setCellWidget(7,9,(QWidget*)comboxList[7]);

    comboxList[10]->setFont(font1);
    comboxList[10]->insertItems(0,list2);
    ui->tableWidget->setCellWidget(10,9,(QWidget*)comboxList[10]);

    comboxList[13]->setFont(font1);
    comboxList[13]->insertItems(0,list1);
    ui->tableWidget->setCellWidget(13,9,(QWidget*)comboxList[13]);

    comboxList[16]->setFont(font1);
    comboxList[16]->insertItems(0,list3);
    ui->tableWidget->setCellWidget(16,9,(QWidget*)comboxList[16]);

    comboxList[19]->setFont(font1);
    comboxList[19]->insertItems(0,list1);
    ui->tableWidget->setCellWidget(19,9,(QWidget*)comboxList[19]);

    /*
    QDoubleSpinBox *ff[105];
    QSpinBox *tt[42];
    int k=0;
    for(int i=0;i<21;i++)
        for(int j=2;j<7;j++)
        {
            ff[k] = new QDoubleSpinBox();
            ff[k]->setRange(0.0,999999999.9);
            ff[k]->setDecimals(2);
            ff[k]->setSingleStep(0.01);
            ff[k]->setValue(0.0);
            ff[k]->setFrame(false);
            ff[k]->setAlignment(Qt::AlignCenter);
            ff[k]->setButtonSymbols(QAbstractSpinBox::NoButtons);
            ui->tableWidget->setCellWidget(i,j,(QWidget*)ff[k]);

            connect(ui->tableWidget->cellWidget(i,j), SIGNAL(), this, SLOT(checkData1(QTableWidgetItem*)));
            k++;
        }

    k = 0;
    for(int i=0;i<21;i++)
        for(int j=7;j<9;j++)
        {
            tt[k] = new QSpinBox();
            tt[k]->setRange(0,9);
            tt[k]->setSingleStep(1);
            tt[k]->setValue(2);
            tt[k]->setFrame(false);
            tt[k]->setAlignment(Qt::AlignCenter);
            tt[k]->setButtonSymbols(QAbstractSpinBox::NoButtons);
            ui->tableWidget->setCellWidget(i,j,(QWidget*)tt[k]);
            k++;
        }
*/    
}

void range::onChanged(double value)
{
    qDebug()<<value<<ui->tableWidget->currentColumn()<<ui->tableWidget->currentRow();


}

void range::checkData1(QTableWidgetItem *item)
{
    qDebug()<<item->row()<<item->column();

}

void range::checkData(int row,int column)
{
    changed = 1;
    QString t = ui->tableWidget->item(row,column)->text();
    if(column == 7 || column == 8)
    {
        QRegExp rx("^\\d+$");
        if(!rx.exactMatch(t))
        {
            disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            ui->tableWidget->item(row,column)->setText("0");
            connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
        }else {
            int prec = ui->tableWidget->item(row,8)->text().toInt();
            int prec2 = ui->tableWidget->item(row,7)->text().toInt();

            if(prec2 > prec)
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("默认精度不能大于精度设定值");
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                ui->tableWidget->item(row,column)->setText("0");
                return;
            }

            if(column == 8)
            {
                if(!ui->tableWidget->item(row,2)->text().isEmpty())
                    ui->tableWidget->item(row,2)->setText(QString::number(ui->tableWidget->item(row,2)->text().toFloat(),10,prec));

                if(!ui->tableWidget->item(row,3)->text().isEmpty())
                    ui->tableWidget->item(row,3)->setText(QString::number(ui->tableWidget->item(row,3)->text().toFloat(),10,prec));

                if(!ui->tableWidget->item(row,4)->text().isEmpty())
                    ui->tableWidget->item(row,4)->setText(QString::number(ui->tableWidget->item(row,4)->text().toFloat(),10,prec));

                if(!ui->tableWidget->item(row,5)->text().isEmpty())
                    ui->tableWidget->item(row,5)->setText(QString::number(ui->tableWidget->item(row,5)->text().toFloat(),10,prec));

                if(!ui->tableWidget->item(row,6)->text().isEmpty())
                    ui->tableWidget->item(row,6)->setText(QString::number(ui->tableWidget->item(row,6)->text().toFloat(),10,prec));
            }
        }
    }else if(column >= 2 && column <= 6){
        QRegExp rx("^\\d+\(\\.\\d+\)?$");
        int prec1 = ui->tableWidget->item(row,8)->text().toInt();
        if(!rx.exactMatch(t))
        {
            disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            ui->tableWidget->item(row,column)->setText(QString::number(0.0,10,prec1));
            connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
        }else{
            disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
            ui->tableWidget->item(row,column)->setText(QString::number(t.toFloat(),10,prec1));
            connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
        }
    }
}

/*************************************
 * 函数名：readRangeFromDB
 * 说明：
*************************************/
void range::readRangeFromDB(range_Item set[])
{
    QFont font("Microsoft YaHei",12,QFont::Normal);
    QStringList list;
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");   //设置数据库驱动
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
        set[i].id = q.value(1).toInt();
        set[i].active = q.value(2).toInt();

        set[i].def[0] = q.value(3).toFloat();
        set[i].minDefault[0] = q.value(4).toFloat();
        set[i].min[0] = q.value(5).toFloat();
        set[i].maxDefault[0] = q.value(6).toFloat();
        set[i].max[0] = q.value(7).toFloat();
        set[i].precDefault[0] = q.value(8).toInt();
        set[i].prec[0] = q.value(9).toInt();
        set[i].unit[0] = q.value(10).toString();

        set[i].def[1] = q.value(11).toFloat();
        set[i].minDefault[1] = q.value(12).toFloat();
        set[i].min[1] = q.value(13).toFloat();
        set[i].maxDefault[1] = q.value(14).toFloat();
        set[i].max[1] = q.value(15).toFloat();
        set[i].precDefault[1] = q.value(16).toInt();
        set[i].prec[1] = q.value(17).toInt();
        set[i].unit[1] = q.value(18).toString();

        set[i].def[2] = q.value(19).toFloat();
        set[i].minDefault[2] = q.value(20).toFloat();
        set[i].min[2] = q.value(21).toFloat();
        set[i].maxDefault[2] = q.value(22).toFloat();
        set[i].max[2] = q.value(23).toFloat();
        set[i].precDefault[2] = q.value(24).toInt();
        set[i].prec[2] = q.value(25).toInt();
        set[i].unit[2] = q.value(26).toString();

        set[i].def[3] = q.value(27).toFloat();
        set[i].minDefault[3] = q.value(28).toFloat();
        set[i].min[3] = q.value(29).toFloat();
        set[i].maxDefault[3] = q.value(30).toFloat();
        set[i].max[3] = q.value(31).toFloat();
        set[i].precDefault[3] = q.value(32).toInt();
        set[i].prec[3] = q.value(33).toInt();
        set[i].unit[3] = q.value(34).toString();
        i++;
    }
    db.close();

    for(int i=0;i<7;i++)
        for(int j=0;j<3;j++)
    {
        ui->tableWidget->item(i*3+j,2)->setText(QString::number(set[i].def[j],10,set[i].prec[j]));
        ui->tableWidget->item(i*3+j,3)->setText(QString::number(set[i].minDefault[j],10,set[i].prec[j]));
        ui->tableWidget->item(i*3+j,4)->setText(QString::number(set[i].min[j],10,set[i].prec[j]));
        ui->tableWidget->item(i*3+j,5)->setText(QString::number(set[i].maxDefault[j],10,set[i].prec[j]));
        ui->tableWidget->item(i*3+j,6)->setText(QString::number(set[i].max[j],10,set[i].prec[j]));
        ui->tableWidget->item(i*3+j,7)->setText(QString::number(set[i].precDefault[j],10));
        ui->tableWidget->item(i*3+j,8)->setText(QString::number(set[i].prec[j],10));

        list.clear();
        if(j == 0)
        {
            list<<"KV"<<"V"<<"mV";
        }else if(j == 2){
            list<<"S"<<"mS";
        }else if(j == 1 && (i == 0 || i == 6 || i == 4)){
            list<<"A"<<"mA"<<"μA";
        }else if(j == 1 && (i == 1 || i == 2 || i == 3)){
            list<<"MΩ"<<"kΩ"<<"Ω";
        }else if(j == 1 && i == 5){
            list<<"kW"<<"W";
        }

        if(set[i].active == 1)
            ui->tableWidget->item(i*3,0)->setCheckState(Qt::Checked);
        else
            ui->tableWidget->item(i*3,0)->setCheckState(Qt::Unchecked);

        QComboBox *it8 = new QComboBox();
        it8->setFont(font);
        it8->insertItems(0,list);
        it8->setCurrentText(set[i].unit[j]);
        ui->tableWidget->setCellWidget(i*3+j,9,(QWidget*)it8);
    }
    connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(checkData(int,int)));
}

/*************************************
 * 函数名：     saveRange
 * 说明：
*************************************/
void range::saveRange()
{   
    range_Item set[7];

    for(int i=0;i<7;i++)
        for(int j=0;j<3;j++)
        {
            set[i].def[j] = ui->tableWidget->item(i*3+j,2)->text().toFloat();
            set[i].minDefault[j] = ui->tableWidget->item(i*3+j,3)->text().toFloat();
            set[i].min[j] = ui->tableWidget->item(i*3+j,4)->text().toFloat();
            set[i].maxDefault[j] = ui->tableWidget->item(i*3+j,5)->text().toFloat();
            set[i].max[j] = ui->tableWidget->item(i*3+j,6)->text().toFloat();
            set[i].precDefault[j] = ui->tableWidget->item(i*3+j,7)->text().toInt();
            set[i].prec[j] = ui->tableWidget->item(i*3+j,8)->text().toInt();

            QWidget *widget = ui->tableWidget->cellWidget(i*3+j,9);
            QComboBox *combox = (QComboBox*)widget;
            set[i].unit[j] = combox->currentText();

            if(set[i].min[j] > set[i].max[j])
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("下限大于上限");
                ui->tableWidget->item(i*3+j,4)->setSelected(true);
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                return;
            }

            if(set[i].def[j] < set[i].min[j])
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("默认值低于下限");
                ui->tableWidget->item(i*3+j,2)->setSelected(true);
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                return;
            }

            if(set[i].def[j] > set[i].max[j])
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("默认值大于上限");
                ui->tableWidget->item(i*3+j,2)->setSelected(true);
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                return;
            }

            if(set[i].minDefault[j] > set[i].max[j])
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("默认下限大于上限");
                ui->tableWidget->item(i*3+j,3)->setSelected(true);
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                return;
            }

            if(set[i].minDefault[j] < set[i].min[j])
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("默认下限小于下限");
                ui->tableWidget->item(i*3+j,3)->setSelected(true);
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                return;
            }

            if(set[i].maxDefault[j] > set[i].max[j])
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("默认上限大于上限");
                ui->tableWidget->item(i*3+j,5)->setSelected(true);
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                return;
            }

            if(set[i].maxDefault[j] < set[i].min[j])
            {
                QMessageBox *messageBox = new QMessageBox(this);
                messageBox->setText("默认上限小于下限");
                ui->tableWidget->item(i*3+j,5)->setSelected(true);
                messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
                messageBox->exec();
                return;
            }
        }

    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";

    //连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");   //设置数据库驱动
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db.setDatabaseName(dsn);    //设置连接字符串
    db.setUserName("");     //设置登陆数据库的用户名
    db.setPassword("");     //设置密码
    bool ok = db.open();

    QSqlQuery query;
    bool isSucceed;

    for(int i=0;i<7;i++)
    {
        if(ui->tableWidget->item(i*3,0)->checkState() == Qt::Checked)
            set[i].active = 1;
        else
            set[i].active = 0;

        for(int j=0;j<3;j++)
        {
            set[i].def[j] = ui->tableWidget->item(i*3+j,2)->text().toFloat();
            set[i].minDefault[j] = ui->tableWidget->item(i*3+j,3)->text().toFloat();
            set[i].min[j] = ui->tableWidget->item(i*3+j,4)->text().toFloat();
            set[i].maxDefault[j] = ui->tableWidget->item(i*3+j,5)->text().toFloat();
            set[i].max[j] = ui->tableWidget->item(i*3+j,6)->text().toFloat();
            set[i].precDefault[j] = ui->tableWidget->item(i*3+j,7)->text().toInt();
            set[i].prec[j] = ui->tableWidget->item(i*3+j,8)->text().toInt();

            QWidget *widget = ui->tableWidget->cellWidget(i*3+j,9);
            QComboBox *combox = (QComboBox*)widget;
            set[i].unit[j] = combox->currentText();
        }

        if(i == 5 || i == 6)
        {
            set[i].def[3] = ui->tableWidget->item(i*3+3,2)->text().toFloat();
            set[i].minDefault[3] = ui->tableWidget->item(i*3+3,3)->text().toFloat();
            set[i].min[3] = ui->tableWidget->item(i*3+3,4)->text().toFloat();
            set[i].maxDefault[3] = ui->tableWidget->item(i*3+3,5)->text().toFloat();
            set[i].max[3] = ui->tableWidget->item(i*3+3,6)->text().toFloat();
            set[i].precDefault[3] = ui->tableWidget->item(i*3+3,7)->text().toInt();
            set[i].prec[3] = ui->tableWidget->item(i*3+3,8)->text().toInt();

            QWidget *widget = ui->tableWidget->cellWidget(i*3+3,9);
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

        if(i == 5)
        {
            query.bindValue(":v1",set[i].active);
            query.bindValue(":v2",set[i].def[0]);
            query.bindValue(":v3",set[i].minDefault[0]);
            query.bindValue(":v4",set[i].min[0]);
            query.bindValue(":v5",set[i].maxDefault[0]);
            query.bindValue(":v6",set[i].max[0]);
            query.bindValue(":v7",set[i].precDefault[0]);
            query.bindValue(":v8",set[i].prec[0]);
            query.bindValue(":v9",set[i].unit[0]);

            query.bindValue(":v10",set[i].def[3]);
            query.bindValue(":v11",set[i].minDefault[3]);
            query.bindValue(":v12",set[i].min[3]);
            query.bindValue(":v13",set[i].maxDefault[3]);
            query.bindValue(":v14",set[i].max[3]);
            query.bindValue(":v15",set[i].precDefault[3]);
            query.bindValue(":v16",set[i].prec[3]);
            query.bindValue(":v17",set[i].unit[3]);

            query.bindValue(":v18",set[i].def[1]);
            query.bindValue(":v19",set[i].minDefault[1]);
            query.bindValue(":v20",set[i].min[1]);
            query.bindValue(":v21",set[i].maxDefault[1]);
            query.bindValue(":v22",set[i].max[1]);
            query.bindValue(":v23",set[i].precDefault[1]);
            query.bindValue(":v24",set[i].prec[1]);
            query.bindValue(":v25",set[i].unit[1]);

            query.bindValue(":v26",set[i].def[2]);
            query.bindValue(":v27",set[i].minDefault[2]);
            query.bindValue(":v28",set[i].min[2]);
            query.bindValue(":v29",set[i].maxDefault[2]);
            query.bindValue(":v30",set[i].max[2]);
            query.bindValue(":v31",set[i].precDefault[2]);
            query.bindValue(":v32",set[i].prec[2]);
            query.bindValue(":v33",set[i].unit[2]);

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
            QMessageBox messageBox;
            messageBox.setText(se.text());
            messageBox.exec();
        }
    }

    db.close();
    changed = 0;
}

/*************************************
 * 函数名：     exitRange
 * 说明：
*************************************/
void range::exitRange()
{
    if(changed == 1)
    {
        int i = QMessageBox::information( this, "量程设置","量程已经修改未保存，确定退出？","&确定", "&取消",0,1);
        if(i == 1)
        {
            saveRange();
        }
    }
    this->close();
}
