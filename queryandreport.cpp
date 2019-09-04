#include "queryandreport.h"
#include "ui_queryandreport.h"

queryAndReport::queryAndReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::queryAndReport)
{
    ui->setupUi(this);

    seDate = QString("%1～%1").arg(QDate::currentDate().toString("yyyy-MM-dd"));
    ui->progressBar->setVisible(false);

    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(doQuery()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(makeReportQuickly()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(on_exit_clicked()));
    connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(readTestData(QTableWidgetItem*)));
}

queryAndReport::~queryAndReport()
{
    delete ui;
}

void queryAndReport::iniQuery(QString name)
{
    userName = name;

    QDate dt = QDate::currentDate();
    ui->dateEdit->setDate(dt);
    ui->dateEdit_2->setDate(dt);    

    int width = ui->tableWidget->width();
    ui->tableWidget->setColumnWidth(0,width/4);
    ui->tableWidget->setColumnWidth(1,width/4);
    ui->tableWidget->setColumnWidth(2,width/8);
    ui->tableWidget->setColumnWidth(3,width/4);
    ui->tableWidget->setColumnWidth(4,(width/8)-ui->tableWidget->verticalScrollBar()->width());

    width = ui->tableWidget_2->width();
    ui->tableWidget_2->setColumnWidth(0,width/10);
    ui->tableWidget_2->setColumnWidth(1,width*7/20);
    ui->tableWidget_2->setColumnWidth(2,width*3/20);
    ui->tableWidget_2->setColumnWidth(3,width*3/20);
    ui->tableWidget_2->setColumnWidth(4,width*3/20);
    ui->tableWidget_2->setColumnWidth(5,(width/10)-ui->tableWidget_2->verticalScrollBar()->width());
}

void queryAndReport::on_exit_clicked()
{
    this->close();
    this->deleteLater();
}

void queryAndReport::readTestData(QTableWidgetItem* item)
{
    QString tTime = ui->tableWidget->item(item->row(),3)->text();
    QSqlDatabase db;
    QString sDbNm = QCoreApplication::applicationDirPath()+"/data/"+tTime.mid(0,10)+".mdb";      //数据库文件
    if(!QFile::exists(sDbNm))
        return;

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

    QFont font("Microsoft YaHei",12,QFont::Normal);
    ui->tableWidget_2->setRowCount(0);
    int row = 0;
    QSqlQuery q;
    QString sql = QString("select * from TestData where recordTime='%1' order by sequence;").arg(tTime);
    q.exec(sql);
    while(q.next())
    {
        ui->tableWidget_2->insertRow(row);
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setFont(font);
        item->setTextAlignment(Qt::AlignCenter);
        item->setText(q.value(4).toString());
        ui->tableWidget_2->setItem(row,0,item);

        QString info = QString("%1%2 %3%4~%5%6 %7%8").arg(q.value(5).toString()).arg(q.value(6).toString())
                .arg(q.value(7).toString()).arg(q.value(8).toString()).arg(q.value(9).toString())
                .arg(q.value(10).toString()).arg(q.value(11).toString()).arg(q.value(12).toString());

        if(q.value(4).toString() == tr("泄漏"))
        {
            QStringList cdList = q.value(13).toString().split("|");
            if(cdList.count() == 2)
            {
                if(cdList.at(0) == "1")
                {
                    if(cdList.at(1) == "0")
                        info += " ABC";
                    else if(cdList.at(1) == "1")
                        info += " BAC";
                    else if(cdList.at(1) == "2")
                        info += " BC";
                    else if(cdList.at(1) == "3")
                        info += " AC";
                    else if(cdList.at(1) == "4")
                        info += " AB";
                }else{
                    if(cdList.at(1) == "0")
                        info += tr(" 静态");
                    else if(cdList.at(1) == "1")
                        info += tr(" 动态");
                }
            }
        }

        QTableWidgetItem *item1 = new QTableWidgetItem();
        item1->setFont(font);
        item1->setTextAlignment(Qt::AlignCenter);
        item1->setText(info);
        ui->tableWidget_2->setItem(row,1,item1);

        info = QString("%1%2").arg(q.value(16).toString()).arg(q.value(19).toString());
        QTableWidgetItem *item2 = new QTableWidgetItem();
        item2->setFont(font);
        item2->setTextAlignment(Qt::AlignCenter);
        item2->setText(info);
        ui->tableWidget_2->setItem(row,2,item2);

        info = QString("%1%2").arg(q.value(17).toString()).arg(q.value(20).toString());
        QTableWidgetItem *item3 = new QTableWidgetItem();
        item3->setFont(font);
        item3->setTextAlignment(Qt::AlignCenter);
        item3->setText(info);
        ui->tableWidget_2->setItem(row,3,item3);

        info = QString("%1%2").arg(q.value(18).toString()).arg(q.value(21).toString());
        QTableWidgetItem *item4 = new QTableWidgetItem();
        item4->setFont(font);
        item4->setTextAlignment(Qt::AlignCenter);
        item4->setText(info);
        ui->tableWidget_2->setItem(row,4,item4);

        QTableWidgetItem *item5 = new QTableWidgetItem();
        item5->setFont(font);
        item5->setTextAlignment(Qt::AlignCenter);

        if(q.value(14).toString() == "0")
            item5->setText(tr("不合格"));
        else
            item5->setText(tr("合格"));
        ui->tableWidget_2->setItem(row,5,item5);

        row++;
    }
    q.clear();
    db.close();
}

void queryAndReport::makeReportQuickly()
{
    if(ui->tableWidget->rowCount() == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("数据查询"),tr("请先进行查询再导出报表！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(0);
    int num = ui->tableWidget->rowCount();
    int pass = 0;
    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString saveTime1 = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QAxObject *excel = new QAxObject();
    if(excel->setControl("Excel.Application"))
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QAxObject *col;

        excel->dynamicCall("SetVisible(bool Visible)","false");
        excel->setProperty("DisplayAlerts", false);
        QAxObject *workbooks = excel->querySubObject("WorkBooks");
        workbooks->dynamicCall("Add");
        QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
        QAxObject *worksheet = workbook->querySubObject("Worksheets(int)",1);

        QAxObject *range = worksheet->querySubObject("Range(const QString&)", "D1:F1");
        range->setProperty("MergeCells", true);

        range = worksheet->querySubObject("Range(const QString&)", "A1:F3");
        range->setProperty("NumberFormatLocal", "@");
        range = range->querySubObject("Font");
        range->setProperty("Bold", true);

        col = worksheet->querySubObject("Columns(const QString&)", "A:A");
        col->setProperty("ColumnWidth", 9.38);
        col = worksheet->querySubObject("Columns(const QString&)", "B:B");
        col->setProperty("ColumnWidth", 21.88);
        col = worksheet->querySubObject("Columns(const QString&)", "C:E");
        col->setProperty("ColumnWidth", 13.13);
        col = worksheet->querySubObject("Columns(const QString&)", "F:F");
        col->setProperty("ColumnWidth", 7.0);

        QList<QList<QVariant>> e_data;
        QList<QVariant> rows;
        rows<<tr("制表时间:")<<saveTime<<tr("起止日期:")<<seDate<<""<<"";
        e_data.append(rows);
        rows.clear();
        rows<<tr("制表人:")<<userName<<tr("总数:")<<QString("%1").arg(num)<<tr("合格率:")<<"";
        e_data.append(rows);
        rows.clear();
        rows<<tr("合格数:")<<""<<tr("不合格数:")<<""<<""<<"";
        e_data.append(rows);
        rows.clear();
        rows<<""<<""<<""<<""<<""<<"";
        e_data.append(rows);
        rows.clear();
        rows<<""<<""<<""<<""<<""<<"";
        e_data.append(rows);

        QVariantList vars;
        int rowNum = e_data.size();
        for(int i=0;i<rowNum;++i)
        {
            vars.append(QVariant(e_data[i]));
        }
        QVariant res = QVariant(vars);

        range = worksheet->querySubObject("Range(const QString&)", "A1:F5");
        range->setProperty("NumberFormatLocal", "@");
        range->setProperty("HorizontalAlignment",-4108);
        range->setProperty("VerticalAlignment",-4108);
        range->setProperty("WrapText", true);
        range->setProperty("Value", res);               

        int row = 6;
        QString sDbNm = "";
        QSqlDatabase db;
        if(QSqlDatabase::contains("qt_sql_default_connection"))
            db = QSqlDatabase::database("qt_sql_default_connection");
        else
            db = QSqlDatabase::addDatabase("QODBC");

        for(int i=0;i<num;i++)
        {
            QApplication::processEvents();
            ui->progressBar->setValue((int)(((i*1.0)/(num*1.0))*100.0));
            QString tTime = ui->tableWidget->item(i,3)->text();
            QString tem = QCoreApplication::applicationDirPath()+"/data/"+tTime.mid(0,10)+".mdb";
            if(tem != sDbNm)
            {
                if(db.isOpen())
                    db.close();
                sDbNm = tem;

                QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;")
                        .arg(sDbNm);    //连接字符串
                db.setDatabaseName(dsn);    //设置连接字符串
                db.setUserName("");     //设置登陆数据库的用户名
                db.setPassword("");     //设置密码
                bool ok = db.open();

                if(!ok)
                {
                    continue;
                }
            }           

            range = worksheet->querySubObject("Range(const QString&)", QString("D%1:F%1").arg(row+1));
            range->setProperty("MergeCells", true);

            range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row+2));            
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
            range = range->querySubObject("Font");
            range->setProperty("Bold", true);

            range = worksheet->querySubObject("Range(const QString&)", QString("D%1:F%1").arg(row+1));
            range->setProperty("NumberFormatLocal", "@");

            e_data.clear();

            range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row));
            range->setProperty("NumberFormatLocal", "@");
            rows.clear();
            rows<<tr("测试时间:")<<tTime<<tr("操作员:")<<ui->tableWidget->item(i,2)->text()<<tr("测试结果:")<<ui->tableWidget->item(i,4)->text();
            range->setProperty("Value", QVariant(rows));

            range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row+1));
            rows.clear();
            rows<<tr("测试程序:")<<ui->tableWidget->item(i,1)->text()<<tr("条码:")<<ui->tableWidget->item(i,0)->text()<<""<<"";
            range->setProperty("Value", QVariant(rows));

            range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row+2));
            rows.clear();
            rows<<tr("测试项")<<tr("测试条件")<<tr("测试值")<<tr("测试值")<<tr("测试值")<<tr("结论");
            range->setProperty("Value", QVariant(rows));

            if(ui->tableWidget->item(i,4)->text() == tr("合格"))
                pass++;

            int pwNum = 0;
            int con = 0;
            QSqlQuery q;
            QString sql = QString("select * from TestData where recordTime='%1' order by sequence;").arg(tTime);
            q.exec(sql);
            while(q.next())
            {
                QApplication::processEvents();

                range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row+3+con));
                range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
                range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));

                QString info = q.value(4).toString();
                if(tc.machineModel == 2 && info == tr("功率"))
                {
                    pwNum++;
                    info = tr("功率") + QString("%1").arg(pwNum);
                }

                QString info1 = QString("%1%2 %3%4~%5%6 %7%8").arg(q.value(5).toString()).arg(q.value(6).toString())
                        .arg(q.value(7).toString()).arg(q.value(8).toString()).arg(q.value(9).toString())
                        .arg(q.value(10).toString()).arg(q.value(11).toString()).arg(q.value(12).toString());

                QStringList cdList = q.value(13).toString().split("|");
                if(cdList.at(1) == "0")
                    info1 += tr(" 静态");
                else if(cdList.at(1) == "1")
                    info1 += tr(" 动态");

                QString info2 = QString("%1%2").arg(q.value(16).toString()).arg(q.value(19).toString());
                QString info3 = QString("%1%2").arg(q.value(17).toString()).arg(q.value(20).toString());
                QString info4 = QString("%1%2").arg(q.value(18).toString()).arg(q.value(21).toString());
                QString info5;
                if(q.value(14).toString() == "0")
                    info5 = tr("不合格");
                else
                    info5 = tr("合格");
                rows.clear();
                rows<<info<<info1<<info2<<info3<<info4<<info5;
                range->setProperty("Value", QVariant(rows));
                e_data.append(rows);
                con++;
            }
            q.clear();
            QApplication::processEvents();

            range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row).arg(row+3+con));

            row += con+5;
        }
        if(db.isOpen())
            db.close();

        range = worksheet->querySubObject("Range(const QString&)", QString("A1:F%1").arg(row-1));
        range->setProperty("HorizontalAlignment",-4108);
        range->setProperty("VerticalAlignment",-4108);
        range->setProperty("WrapText", true);

        if(num > 0 && pass <= num)
        {
            QString r = QString::number(((float)pass/(float)num)*100.0,10,2)+"%";
            worksheet->querySubObject("Cells(int,int)",2,6)->dynamicCall("SetValue(const QString&)",r);
            worksheet->querySubObject("Cells(int,int)",3,2)->dynamicCall("SetValue(const QString&)",QString("%1").arg(pass));
            worksheet->querySubObject("Cells(int,int)",3,4)->dynamicCall("SetValue(const QString&)",QString("%1").arg(num-pass));
        }

        QString fileName = QCoreApplication::applicationDirPath()+"/report/"+saveTime1+".xlsx";
        workbook->dynamicCall("SaveAs(const QString &)",QDir::toNativeSeparators(fileName));
        workbook->dynamicCall("Close()");
        excel->dynamicCall("Quit()");
        delete range;
        delete excel;
        excel=NULL;
        ui->progressBar->setValue(100);
        QApplication::restoreOverrideCursor();

        QMessageBox box(QMessageBox::Question,tr("导出完成"),tr("文件已经导出，是否现在打开？"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,tr("打开"));
        box.setButtonText(QMessageBox::Cancel,tr("不打开"));
        int ret = box.exec();
        if(ret == QMessageBox::Ok)
        {
            QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
        }
    }else{
        QMessageBox box(QMessageBox::Warning,tr("数据查询"),tr("未能创建 Excel 对象，请安装 Microsoft Excel！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
    }
    ui->progressBar->setVisible(false);
}

void queryAndReport::getReport()
{
    if(ui->tableWidget->rowCount() == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("数据查询"),tr("请先进行查询再导出报表！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    ui->progressBar->setVisible(true);
    ui->progressBar->setValue(0);
    int num = ui->tableWidget->rowCount();
    int pass = 0;
    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString saveTime1 = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QAxObject *excel = new QAxObject();
    if(excel->setControl("Excel.Application"))
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        QAxObject *col;

        excel->dynamicCall("SetVisible(bool Visible)","false");
        excel->setProperty("DisplayAlerts", false);
        QAxObject *workbooks = excel->querySubObject("WorkBooks");
        workbooks->dynamicCall("Add");
        QAxObject *workbook = excel->querySubObject("ActiveWorkBook");
        QAxObject *worksheet = workbook->querySubObject("Worksheets(int)",1);

        QAxObject *range = worksheet->querySubObject("Range(const QString&)", "D1:F1");
        range->setProperty("MergeCells", true);

        range = worksheet->querySubObject("Range(const QString&)", "A1:F3");
        range->setProperty("NumberFormatLocal", "@");
        range = range->querySubObject("Font");
        range->setProperty("Bold", true);        

        col = worksheet->querySubObject("Columns(const QString&)", "A:A");
        col->setProperty("ColumnWidth", 9.38);
        col = worksheet->querySubObject("Columns(const QString&)", "B:B");
        col->setProperty("ColumnWidth", 21.88);
        col = worksheet->querySubObject("Columns(const QString&)", "C:E");
        col->setProperty("ColumnWidth", 13.13);
        col = worksheet->querySubObject("Columns(const QString&)", "F:F");
        col->setProperty("ColumnWidth", 7.0);

        worksheet->querySubObject("Cells(int,int)",1,1)->dynamicCall("SetValue(const QString&)",tr("制表时间:"));
        worksheet->querySubObject("Cells(int,int)",1,2)->dynamicCall("SetValue(const QString&)",saveTime);
        worksheet->querySubObject("Cells(int,int)",1,3)->dynamicCall("SetValue(const QString&)",tr("起止日期:"));
        worksheet->querySubObject("Cells(int,int)",1,4)->dynamicCall("SetValue(const QString&)",seDate);
        worksheet->querySubObject("Cells(int,int)",2,1)->dynamicCall("SetValue(const QString&)",tr("制表人:"));
        worksheet->querySubObject("Cells(int,int)",2,2)->dynamicCall("SetValue(const QString&)",userName);
        worksheet->querySubObject("Cells(int,int)",2,3)->dynamicCall("SetValue(const QString&)",tr("总数:"));
        worksheet->querySubObject("Cells(int,int)",2,4)->dynamicCall("SetValue(const QString&)",QString("%1").arg(num));
        worksheet->querySubObject("Cells(int,int)",2,5)->dynamicCall("SetValue(const QString&)",tr("合格率:"));
        worksheet->querySubObject("Cells(int,int)",3,1)->dynamicCall("SetValue(const QString&)",tr("合格数:"));
        worksheet->querySubObject("Cells(int,int)",3,3)->dynamicCall("SetValue(const QString&)",tr("不合格数:"));

        int row = 6;
        QSqlDatabase db;
        if(QSqlDatabase::contains("qt_sql_default_connection"))
            db = QSqlDatabase::database("qt_sql_default_connection");
        else
            db = QSqlDatabase::addDatabase("QODBC");

        for(int i=0;i<num;i++)
        {
            QApplication::processEvents();
            ui->progressBar->setValue((int)(((i*1.0)/(num*1.0))*100.0));
            QString tTime = ui->tableWidget->item(i,3)->text();
            QString sDbNm = QCoreApplication::applicationDirPath()+"/data/"+tTime.mid(0,10)+".mdb";
            QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;")
                    .arg(sDbNm);//连接字符串
            db.setDatabaseName(dsn);    //设置连接字符串
            db.setUserName("");     //设置登陆数据库的用户名
            db.setPassword("");     //设置密码
            bool ok = db.open();

            if(!ok)
            {
                continue;
            }

            range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%2").arg(row).arg(row+2));
            range->setProperty("NumberFormatLocal", "@");

            worksheet->querySubObject("Cells(int,int)",row,1)->dynamicCall("SetValue(const QString&)",tr("测试时间:"));
            worksheet->querySubObject("Cells(int,int)",row,2)->dynamicCall("SetValue(const QString&)",tTime);
            worksheet->querySubObject("Cells(int,int)",row,3)->dynamicCall("SetValue(const QString&)",tr("操作员:"));
            worksheet->querySubObject("Cells(int,int)",row,4)->dynamicCall("SetValue(const QString&)",ui->tableWidget->item(i,2)->text());
            worksheet->querySubObject("Cells(int,int)",row,5)->dynamicCall("SetValue(const QString&)",tr("测试结果:"));
            worksheet->querySubObject("Cells(int,int)",row,6)->dynamicCall("SetValue(const QString&)",ui->tableWidget->item(i,4)->text());
            worksheet->querySubObject("Cells(int,int)",row+1,1)->dynamicCall("SetValue(const QString&)",tr("测试程序:"));
            worksheet->querySubObject("Cells(int,int)",row+1,2)->dynamicCall("SetValue(const QString&)",ui->tableWidget->item(i,1)->text());
            worksheet->querySubObject("Cells(int,int)",row+1,3)->dynamicCall("SetValue(const QString&)",tr("条码:"));
            worksheet->querySubObject("Cells(int,int)",row+1,4)->dynamicCall("SetValue(const QString&)",ui->tableWidget->item(i,0)->text());

            worksheet->querySubObject("Cells(int,int)",row+2,1)->dynamicCall("SetValue(const QString&)",tr("测试项"));
            worksheet->querySubObject("Cells(int,int)",row+2,2)->dynamicCall("SetValue(const QString&)",tr("测试条件"));
            worksheet->querySubObject("Cells(int,int)",row+2,3)->dynamicCall("SetValue(const QString&)",tr("测试值"));
            worksheet->querySubObject("Cells(int,int)",row+2,4)->dynamicCall("SetValue(const QString&)",tr("测试值"));
            worksheet->querySubObject("Cells(int,int)",row+2,5)->dynamicCall("SetValue(const QString&)",tr("测试值"));
            worksheet->querySubObject("Cells(int,int)",row+2,6)->dynamicCall("SetValue(const QString&)",tr("结论"));

            range = worksheet->querySubObject("Range(const QString&)", QString("D%1:F%1").arg(row+1));
            range->setProperty("MergeCells", true);

            range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row+2));
            range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
            range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
            range = range->querySubObject("Font");
            range->setProperty("Bold", true);

            if(ui->tableWidget->item(i,4)->text() == tr("合格"))
                pass++;

            int pwNum = 0;
            int con = 0;
            QSqlQuery q;
            QString sql = QString("select * from TestData where recordTime='%1' order by sequence;").arg(tTime);
            q.exec(sql);
            while(q.next())
            {
                QApplication::processEvents();

                QString itemName = q.value(4).toString();
                if(tc.machineModel == 2 && itemName == tr("功率"))
                {
                    pwNum++;
                    itemName = tr("功率") + QString("%1").arg(pwNum);
                }
                qDebug()<<itemName;
                range = worksheet->querySubObject("Range(const QString&)", QString("A%1:F%1").arg(row+3+con));
                range->setProperty("NumberFormatLocal", "@");
                range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
                range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));                

                worksheet->querySubObject("Cells(int,int)",row+3+con,1)->dynamicCall("SetValue(const QString&)",itemName);
                QString info = QString("%1%2 %3%4~%5%6 %7%8").arg(q.value(5).toString()).arg(q.value(6).toString())
                        .arg(q.value(7).toString()).arg(q.value(8).toString()).arg(q.value(9).toString())
                        .arg(q.value(10).toString()).arg(q.value(11).toString()).arg(q.value(12).toString());

                if(q.value(13).toString() == "0")
                    info += tr(" 静态");
                else if(q.value(13).toString() == "1")
                    info += tr(" 动态");
                worksheet->querySubObject("Cells(int,int)",row+3+con,2)->dynamicCall("SetValue(const QString&)",info);

                info = QString("%1%2").arg(q.value(16).toString()).arg(q.value(19).toString());
                worksheet->querySubObject("Cells(int,int)",row+3+con,3)->dynamicCall("SetValue(const QString&)",info);

                info = QString("%1%2").arg(q.value(17).toString()).arg(q.value(20).toString());
                worksheet->querySubObject("Cells(int,int)",row+3+con,4)->dynamicCall("SetValue(const QString&)",info);

                info = QString("%1%2").arg(q.value(18).toString()).arg(q.value(21).toString());
                worksheet->querySubObject("Cells(int,int)",row+3+con,5)->dynamicCall("SetValue(const QString&)",info);

                if(q.value(14).toString() == "0")
                    info = tr("不合格");
                else
                    info = tr("合格");
                worksheet->querySubObject("Cells(int,int)",row+3+con,6)->dynamicCall("SetValue(const QString&)",info);

                con++;
            }
            q.clear();
            db.close();
            QApplication::processEvents();
            row += con+5;
        }
        ui->progressBar->setValue(100);
        if(num > 0 && pass <= num)
        {
            QString r = QString::number(((float)pass/(float)num)*100.0,10,2)+"%";
            worksheet->querySubObject("Cells(int,int)",2,6)->dynamicCall("SetValue(const QString&)",r);
            worksheet->querySubObject("Cells(int,int)",3,2)->dynamicCall("SetValue(const QString&)",QString("%1").arg(pass));
            worksheet->querySubObject("Cells(int,int)",3,4)->dynamicCall("SetValue(const QString&)",QString("%1").arg(num-pass));
        }

        range = worksheet->querySubObject("Range(const QString&)", QString("A1:F%1").arg(row));
        range->setProperty("HorizontalAlignment",-4108);
        range->setProperty("VerticalAlignment",-4108);
        range->setProperty("WrapText", true);

        QString fileName = QCoreApplication::applicationDirPath()+"/report/"+saveTime1+".xls";
        workbook->dynamicCall("SaveAs(const QString &)",QDir::toNativeSeparators(fileName));
        workbook->dynamicCall("Close()");
        excel->dynamicCall("Quit()");
        delete range;
        delete excel;
        excel=NULL;

        QApplication::restoreOverrideCursor();

        QMessageBox box(QMessageBox::Question,tr("导出完成"),tr("文件已经导出，是否现在打开？"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
        box.setButtonText(QMessageBox::Ok,tr("打开"));
        box.setButtonText(QMessageBox::Cancel,tr("不打开"));
        int ret = box.exec();
        if(ret == QMessageBox::Ok)
        {
            QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
        }
    }else{
        QMessageBox box(QMessageBox::Warning,tr("数据查询"),tr("未能创建 Excel 对象，请安装 Microsoft Excel！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
    }
    ui->progressBar->setVisible(false);
}

void queryAndReport::doQuery()
{
    QDate startDate = ui->dateEdit->date();
    QDate endDate = ui->dateEdit_2->date();    

    if(startDate > endDate)
    {
        QMessageBox box(QMessageBox::Warning,tr("数据查询"),tr("起始日期不应大于结束日期！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    seDate = QString("%1～%2").arg(startDate.toString("yyyy-MM-dd")).arg(endDate.toString("yyyy-MM-dd"));
    QStringList list;

    if(startDate == endDate)
    {
        list << startDate.toString("yyyy-MM-dd");
    }else{
        QDate dt = startDate;
        int i = 0;
        while(dt < endDate)
        {
            dt = startDate.addDays(i++);
            list << dt.toString("yyyy-MM-dd");
        }
    }

    int a,b;
    if(ui->checkBox->isChecked())
    {
        a = 1;
        if(ui->radioButton->isChecked())
            b = 0;
        else
            b = 1;
    }else{
        a = 0;
    }

    QFont font("Microsoft YaHei",12,QFont::Normal);
    ui->tableWidget->setRowCount(0);
    ui->tableWidget_2->setRowCount(0);

    for(int i=0;i<list.count();i++)
    {
        QSqlDatabase db;
        QString sDbNm = QCoreApplication::applicationDirPath()+"/data/"+list.at(i)+".mdb";
        if(!QFile::exists(sDbNm))
            continue;

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
            continue;
        }

        int row = 0;
        QSqlQuery q;
        QString sql;

        if(a == 0)
        {
            sql = "select * from TestSummary order by id;";
        }else{
            if(b == 0)
            {
                sql= "select * from TestSummary where result='1' order by id;";
            }else{
                sql= "select * from TestSummary where result='0' order by id;";
            }
        }

        q.exec(sql);
        while(q.next())
        {
            ui->tableWidget->insertRow(row);
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setFont(font);
            item->setTextAlignment(Qt::AlignCenter);
            item->setText(q.value(1).toString());
            ui->tableWidget->setItem(row,0,item);

            QTableWidgetItem *item1 = new QTableWidgetItem();
            item1->setFont(font);
            item1->setTextAlignment(Qt::AlignCenter);
            item1->setText(q.value(2).toString());
            ui->tableWidget->setItem(row,1,item1);

            QTableWidgetItem *item2 = new QTableWidgetItem();
            item2->setFont(font);
            item2->setTextAlignment(Qt::AlignCenter);
            item2->setText(q.value(3).toString());
            ui->tableWidget->setItem(row,2,item2);

            QTableWidgetItem *item3 = new QTableWidgetItem();
            item3->setFont(font);
            item3->setTextAlignment(Qt::AlignCenter);
            item3->setText(q.value(5).toString());
            ui->tableWidget->setItem(row,3,item3);

            QTableWidgetItem *item4 = new QTableWidgetItem();
            item4->setFont(font);
            item4->setTextAlignment(Qt::AlignCenter);
            if(q.value(4).toString() == "0")
                item4->setText(tr("不合格"));
            else
                item4->setText(tr("合格"));
            ui->tableWidget->setItem(row,4,item4);

            row++;
        }
        q.clear();
        db.close();
    }
}
