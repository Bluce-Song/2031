#include "parametersenior.h"
#include "ui_parametersenior.h"

parameterSenior::parameterSenior(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameterSenior)
{
    ui->setupUi(this);
    readParameter();
    connect(ui->toolButton_2,SIGNAL(clicked(bool)),this,SLOT(exitSenior()));
}

parameterSenior::~parameterSenior()
{
    delete ui;
}

void parameterSenior::exitSenior()
{
    this->close();
    this->deleteLater();
}

/*************************************
 * 函数名：     saveSenior
 * 说明：
*************************************/
void parameterSenior::saveSenior()
{
    if(ui->radioButton->isChecked())
        tc.language = 0;
    else if(ui->radioButton_2->isChecked())
        tc.language = 1;
    else if(ui->radioButton_5->isChecked())
        tc.language = 2;

    tc.machineModel = ui->comboBox_12->currentIndex();
    tc.phase = ui->comboBox_6->currentIndex();
    tc.itemNumMax = ui->spinBox_8->value();

    if(ui->checkBox_3->checkState() == Qt::Checked)
        tc.md = 1;
    else
        tc.md = 0;

    tc.auxiliary1 = ui->lineEdit_4->text();
    tc.auxiliary2 = ui->lineEdit_14->text();
    tc.auxiliary3 = ui->lineEdit_15->text();
}

void parameterSenior::readParameter()
{
    disconnect(ui->toolButton_14,SIGNAL(clicked(bool)),this,SLOT(saveSenior()));
    if(tc.language == 0)
    {
        ui->radioButton->setChecked(true);
        ui->radioButton_2->setChecked(false);
        ui->radioButton_5->setChecked(false);
    }else if(tc.language == 1){
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(true);
        ui->radioButton_5->setChecked(false);
    }else if(tc.language == 2){
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(false);
        ui->radioButton_5->setChecked(true);
    }

    ui->comboBox_12->setCurrentIndex(tc.machineModel);
    ui->comboBox_6->setCurrentIndex(tc.phase);

    if(tc.md == 1)
        ui->checkBox_3->setCheckState(Qt::Checked);
    else
        ui->checkBox_3->setCheckState(Qt::Unchecked);

    ui->spinBox_8->setValue(tc.itemNumMax);
    ui->lineEdit_4->setText(tc.auxiliary1);
    ui->lineEdit_14->setText(tc.auxiliary2);
    ui->lineEdit_15->setText(tc.auxiliary3);
    connect(ui->toolButton_14,SIGNAL(clicked(bool)),this,SLOT(saveSenior()));
}
