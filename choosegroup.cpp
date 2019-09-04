#include "choosegroup.h"
#include "ui_choosegroup.h"

chooseGroup::chooseGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::chooseGroup)
{
    ui->setupUi(this);
    rowNum = 0;
    setupModel();
    setupView();
    connect(ui->table,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openGroup(QModelIndex)));
    connect(ui->toolButton,SIGNAL(clicked(bool)),this,SLOT(openGroup()));
    connect(ui->toolButton_3,SIGNAL(clicked(bool)),this,SLOT(exitGroup()));
    connect(ui->toolButton_4,SIGNAL(clicked(bool)),this,SLOT(newGroup()));
    connect(ui->toolButton_5,SIGNAL(clicked(bool)),this,SLOT(sendGroupToMainThread()));
}

chooseGroup::~chooseGroup()
{
    delete ui;
}

void chooseGroup::sendGroupToMainThread()
{
    if(rowNum == 0)
        return;

    int gn = model->item(ui->table->currentIndex().row(),0)->text().toInt();
    QString name = model->item(ui->table->currentIndex().row(),1)->text();

    if(type == 0)
    {
        emit sendGroupName(gn,name);
        this->close();
    }else{
        emit sendData(name);
        this->accept();
    }
    this->deleteLater();
}

/*************************************
 * 函数名：     openGroup
 * 说明：
*************************************/
void chooseGroup::openGroup()
{
    if(userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("测试程序"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    if(rowNum == 0)
        return;

    if(type == 1)
        openType = 2;

    QString name = model->item(ui->table->currentIndex().row(),1)->text();
    emit sendData(name);
    this->accept();
    this->deleteLater();
}

/*************************************
 * 函数名：     exitGroup
 * 说明：
*************************************/
void chooseGroup::exitGroup()
{
    this->close();
    this->deleteLater();
}

/*************************************
 * 函数名：     newGroup
 * 说明：
*************************************/
void chooseGroup::newGroup()
{
    if(userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("测试程序"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);       //显示在最前端
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    openType = 1;
    this->accept();
    this->deleteLater();
}

/*************************************
 * 函数名：     openGroup
 * 说明：
*************************************/
void chooseGroup::openGroup(QModelIndex index)
{
    if(userLevel == 0)
    {
        QMessageBox box(QMessageBox::Warning,tr("测试程序"),tr("您无权限进行此操作！"));
        box.setWindowFlags(Qt::WindowStaysOnTopHint);
        box.setStandardButtons(QMessageBox::Ok);
        box.setButtonText(QMessageBox::Ok,tr("确 定"));
        box.exec();
        return;
    }

    if(rowNum == 0)
        return;

    if(type == 1)
        openType = 2;

    QString name = model->item(index.row(),1)->text();
    emit sendData(name);
    this->accept();
    this->deleteLater();
}

/*************************************
 * 函数名：     setupModel
 * 说明：
*************************************/
void chooseGroup::setupModel()
{
    model = new QStandardItemModel(0,3,this);
    model->setHeaderData(0,Qt::Horizontal,tr("组号"));
    model->setHeaderData(1,Qt::Horizontal,tr("程序名"));
    model->setHeaderData(2,Qt::Horizontal,tr("识别码"));
}

/*************************************
 * 函数名：     setupView
 * 说明：
*************************************/
void chooseGroup::setupView()
{
    ui->table->setModel(model);
    ui->table->horizontalHeader()->setStretchLastSection(true);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QItemSelectionModel *selectionModel = new QItemSelectionModel(model);
    ui->table->setSelectionModel(selectionModel);
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),ui->table,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

/*************************************
 * 函数名：     setFillShow
 * 说明：
*************************************/
void chooseGroup::setFillShow()
{
    ui->toolButton_5->setVisible(false);
}

/*************************************
 * 函数名： readSelfCheckGroup
 * 说明：
*************************************/
void chooseGroup::readSelfCheckGroup()
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

    type = 1;
    ui->toolButton_5->setText(tr("补偿测试"));

    QFont font("Microsoft YaHei",12,QFont::Normal);
    QSqlQuery q;
    QString sql = "select ID,TPName,TPTime,IDCode from TestProgram where ID=1;";
    q.exec(sql);

    model->removeRows(0,model->rowCount(QModelIndex()),QModelIndex());
    int row = 0;
    while(q.next())
    {
        model->insertRows(row, 1, QModelIndex());
        model->setData(model->index(row, 0, QModelIndex()),q.value(0).toInt());
        model->setData(model->index(row, 1, QModelIndex()),q.value(1).toString());
        model->setData(model->index(row, 2, QModelIndex()),q.value(3).toString());
        model->item(row,0)->setTextAlignment(Qt::AlignCenter);
        model->item(row,1)->setTextAlignment(Qt::AlignCenter);
        model->item(row,2)->setTextAlignment(Qt::AlignCenter);
        model->item(row,0)->setFont(font);
        model->item(row,1)->setFont(font);
        model->item(row,2)->setFont(font);
        row++;
    }
    db.close();
    rowNum = row;
    if(row > 0)
        ui->table->selectRow(0);
}

/*************************************
 * 函数名： readGroup
 * 说明：
*************************************/
void chooseGroup::readGroup(QString name)
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

    type = 0;
    QFont font("Microsoft YaHei",12,QFont::Normal);
    QSqlQuery q;
    QString sql = "select ID,TPName,TPTime,IDCode from TestProgram where ID<>1 order by ID;";
    q.exec(sql);

    model->removeRows(0,model->rowCount(QModelIndex()),QModelIndex());
    int at = 0;
    int row = 0;
    while(q.next())
    {
        model->insertRows(row, 1, QModelIndex());
        model->setData(model->index(row, 0, QModelIndex()),q.value(0).toInt());
        model->setData(model->index(row, 1, QModelIndex()),q.value(1).toString());
        model->setData(model->index(row, 2, QModelIndex()),q.value(3).toString());
        model->item(row,0)->setTextAlignment(Qt::AlignCenter);
        model->item(row,1)->setTextAlignment(Qt::AlignCenter);
        model->item(row,2)->setTextAlignment(Qt::AlignCenter);
        model->item(row,0)->setFont(font);
        model->item(row,1)->setFont(font);
        model->item(row,2)->setFont(font);

        if(name == q.value(1).toString())
            at = row;

        row++;
    }
    db.close();
    rowNum = row;
    if(row > 0)
        ui->table->selectRow(at);
}
