#include "parameterrange.h"
#include "ui_parameterrange.h"

parameterRange::parameterRange(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameterRange)
{
    ui->setupUi(this);
    readParameter();
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitRange()));
}

parameterRange::~parameterRange()
{
    delete ui;
}

void parameterRange::exitRange()
{
    this->close();
    this->deleteLater();
}

void parameterRange::readRangeFromDB()
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

void parameterRange::readParameter()
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

/*************************************
 * 函数名：     showRange
 * 说明：
*************************************/
void parameterRange::showRange()
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
 * 函数名：     checkItem
 * 说明：
*************************************/
void parameterRange::checkItem(QTableWidgetItem* item)
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
 * 函数名：     checkData
 * 说明：
*************************************/
void parameterRange::checkData(int row,int column)
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
void parameterRange::saveRange()
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

        if(i == 3)
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
