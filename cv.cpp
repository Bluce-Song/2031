#include "cv.h"
#include "ui_cv.h"

CV::CV(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CV)
{
    ui->setupUi(this);
}

CV::~CV()
{
    delete ui;
}

void CV::setTestName(QString name)
{
    ui->label_6->setText(name);
}
