#include "filemanage.h"
#include "ui_filemanage.h"

fileManage::fileManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileManage)
{
    ui->setupUi(this);
}

fileManage::~fileManage()
{
    delete ui;
}
