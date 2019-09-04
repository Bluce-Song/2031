#include "test_new.h"
#include "ui_test_new.h"

test_new::test_new(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::test_new)
{
    ui->setupUi(this);
}

test_new::~test_new()
{
    delete ui;
}
