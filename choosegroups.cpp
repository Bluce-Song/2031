#include "choosegroups.h"
#include "ui_choosegroups.h"

chooseGroups::chooseGroups(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chooseGroups)
{    
    ui->setupUi(this);
    setupModel();
    setupView();
    connect(ui->table,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openGroup(QModelIndex)));
}

chooseGroups::~chooseGroups()
{
    delete ui;
}

/*************************************
 * 函数名：     openGroup
 * 说明：
*************************************/
void chooseGroups::openGroup(QModelIndex index)
{
    qDebug()<<index.row()<<index.column();
    QMessageBox *messageBox = new QMessageBox(this);
    messageBox->setText(index.data().toString());
    messageBox->setStyleSheet("background-color: rgb(236, 233, 216);");
    messageBox->exec();
}

/*************************************
 * 函数名：     setupModel
 * 说明：
*************************************/
void chooseGroups::setupModel()
{
    model = new QStandardItemModel(0,3,this);
    model->setHeaderData(0,Qt::Horizontal,"组号");
    model->setHeaderData(1,Qt::Horizontal,"组名");
    model->setHeaderData(2,Qt::Horizontal,"时间");
}

/*************************************
 * 函数名：     setupView
 * 说明：
*************************************/
void chooseGroups::setupView()
{
    ui->table->setModel(model);
    ui->table->horizontalHeader()->setStretchLastSection(true);
    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QItemSelectionModel *selectionModel = new QItemSelectionModel(model);
    ui->table->setSelectionModel(selectionModel);
    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),ui->table,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

/*************************************
 * 函数名： readGroup
 * 说明：   读取所有测试组信息
*************************************/
void chooseGroups::readGroup()
{
    //连接数据库
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";      //数据库文件
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
    QString sql = "select ID,TPName,TPTime,IDCode from TestProgram order by ID";
    q.exec(sql);

    model->removeRows(0,model->rowCount(QModelIndex()),QModelIndex());
    int row = 0;
    while(q.next())
    {
        model->insertRows(row, 1, QModelIndex());        
        model->setData(model->index(row, 0, QModelIndex()),q.value(0).toInt());
        model->setData(model->index(row, 1, QModelIndex()),q.value(1).toString());
        model->setData(model->index(row, 2, QModelIndex()),q.value(2).toString());
        row++;
    }
    db.close();
}
