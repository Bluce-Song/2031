#include "mvtestui.h"
#include "ui_mvtestui.h"
#include <QresizeEvent>

mvTestUI::mvTestUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mvTestUI)
{
    ui->setupUi(this);
    widgetWidth = this->size().width();
    widgetHeight = this->size().height();
    connect(ui->toolButton,SIGNAL(clicked(bool)),this,SLOT(setNewSize()));
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(setNewSize1()));
//    testItem *ti = new testItem(ui->listWidget);
//    QListWidgetItem *alarmItem = new QListWidgetItem(ui->listWidget);
//    ui->listWidget->setItemWidget(alarmItem,ti);
////    ui->listWidget->setGeometry(0,0,300,350);
//        alarmItem->setSizeHint(QSize(0,100));
}

mvTestUI::~mvTestUI()
{
    delete ui;
}

void mvTestUI::resizeEvent(QResizeEvent *size)
{
    if(size->size().width() != widgetWidth)
    {
//        qDebug()<<"width+height"<<size->size().width()<<size->size().height();
//        qDebug()<<"width&height"<<ui->listView->size().width()<<ui->listView->size().height()/8;
        item->setSizeHint(QSize(0,ui->listView->size().height()/8));
        ti->setItemSize(ui->listView->size().width(),ui->listView->size().height()/8);
    }
//    qDebug()<<"finished";
//    centralWidget->resize(frameGeometry().size()); //是采用size()还是frameGeometry.size()根据自己的需求。
}

void mvTestUI::modelViewFun()
{
    qDebug()<<ui->listView->size().width();
    model = new QStandardItemModel(ui->listView);
    ui->listView->setModel(model);
    ti = new testItem(ui->listView);

    item = new QStandardItem();
    model->appendRow(item);
    item->setSizeHint(QSize(0,ui->listView->size().height()/8));    //保证高度
    QModelIndex index = model->indexFromItem(item);
    ui->listView->setIndexWidget(index,ti);
    ti->setItemSize(ui->listView->size().width(),item->sizeHint().height());

//    ti->setItemName("这是一次测试");
}

void mvTestUI::setNewSize()
{
    qDebug()<<ui->listView->size().width();
    ti->setItemSize(ti->size().width()+100,ti->size().height()+10);
    ti->setItemName("这是一次测试");
}

void mvTestUI::setNewSize1()
{
    ti->setItemSize(ti->size().width()-100,ti->size().height()-10);
}

/*************************************
 * 函数名：     setupModel
 * 说明：
*************************************/
void mvTestUI::setupModel()
{
    model = new QStandardItemModel(this);

}

/*************************************
 * 函数名：     setupView
 * 说明：
*************************************/
void mvTestUI::setupView()
{
    ui->listView->setModel(model);


//    connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),ui->table,SLOT(selectionChanged(QItemSelection,QItemSelection)));
}
