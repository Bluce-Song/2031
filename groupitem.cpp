#include "groupitem.h"

groupItem::groupItem(QWidget *parent) : QWidget(parent)
{
    QFont font("Microsoft YaHei",12,QFont::Normal);
    QStringList list,list1;

    if(rangeSets[0].active == 1)
        list<<tr("交耐");
    if(rangeSets[1].active == 1)
        list<<tr("直耐");
    if(rangeSets[2].active == 1)
        list<<tr("绝缘");
    if(rangeSets[3].active == 1)
        list<<tr("接地");
    if(rangeSets[4].active == 1)
        list<<tr("泄漏");
    if(rangeSets[5].active == 1)
        list<<tr("功率");
    if(rangeSets[6].active == 1)
        list<<tr("低启");
    if(rangeSets[7].active == 1)
        list<<tr("开短");

    if(tc.phase == 1)
    {
        list1<<"ABC"<<"BC"<<"AC"<<"AB";
    }else if(tc.phase == 2){
        list1<<"ABC"<<"BC"<<"AC"<<"AB"<<"A"<<"B"<<"C";
    }else{
        list1<<tr("静态")<<tr("动态");
    }
    rowNum = 0;
    phase = 2;
    hWidget = new QWidget(this);
//    gLayout = new QGridLayout(this);
    hLayout = new QHBoxLayout(this);

    itemName = new QComboBox();   
    itemName->setFont(font);    
    itemName->insertItems(0,list);
    itemName->setStyleSheet("background-color: rgb(239, 239, 241);selection-color: rgb(0, 0, 0);");
    itemName->setFrame(true);
    itemName->setMinimumWidth(90);
    itemName1 = new QLabel("");
    itemName1->setStyleSheet("border:0px;");
    vLayout1 = new QVBoxLayout;
    vLayout1->addWidget(itemName1);
    vLayout1->addWidget(itemName);
    vLayout1->setContentsMargins(0,0,0,0);
    vLayout1->setSpacing(0);
    vLayout1->setStretch(1,3);

    data1 = new QDoubleSpinBox();
    data1->setStyleSheet("background-color: rgb(239, 239, 241);");
    data1->setFont(font);
    data1->setButtonSymbols(QAbstractSpinBox::NoButtons);

    unit1 = new QLabel(tr("电压(V)"));
    unit1->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
    unit1->setFont(font);
    unit1->setAlignment(Qt::AlignCenter);
    vLayout2 = new QVBoxLayout;
    vLayout2->addWidget(unit1);
    vLayout2->addWidget(data1);
    vLayout2->setContentsMargins(0,0,0,0);
    vLayout2->setSpacing(0);
    vLayout2->setStretch(1,3);

    data2 = new QDoubleSpinBox();
    data2->setStyleSheet("background-color: rgb(239, 239, 241);");
    data2->setFont(font);
    data2->setButtonSymbols(QAbstractSpinBox::NoButtons);

    unit2 = new QLabel(tr("电流上限(mA)"));
    unit2->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
    unit2->setFont(font);
    unit2->setAlignment(Qt::AlignCenter);
    vLayout3 = new QVBoxLayout;
    vLayout3->addWidget(unit2);
    vLayout3->addWidget(data2);
    vLayout3->setContentsMargins(0,0,0,0);
    vLayout3->setSpacing(0);
    vLayout3->setStretch(1,3);    
//    gLayout->addWidget(unit2,0,2,1,1);
//    gLayout->addWidget(data2,1,2,1,1);

    data3 = new QDoubleSpinBox();
    data3->setStyleSheet("background-color: rgb(239, 239, 241);");
    data3->setFont(font);
    data3->setButtonSymbols(QAbstractSpinBox::NoButtons);

    unit3 = new QLabel("");
    unit3->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
    unit3->setFont(font);
    unit3->setAlignment(Qt::AlignCenter);
    vLayout4 = new QVBoxLayout;
    vLayout4->addWidget(unit3);
    vLayout4->addWidget(data3);
    vLayout4->setContentsMargins(0,0,0,0);
    vLayout4->setSpacing(0);
    vLayout4->setStretch(1,3);
//    gLayout->addWidget(unit3,0,3,1,1);
//    gLayout->addWidget(data3,1,3,1,1);

    data4 = new QDoubleSpinBox();
    data4->setStyleSheet("background-color: rgb(239, 239, 241);");
    data4->setFont(font);
    data4->setButtonSymbols(QAbstractSpinBox::NoButtons);

    unit4 = new QLabel(tr("时间(S)"));
    unit4->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
    unit4->setFont(font);
    unit4->setAlignment(Qt::AlignCenter);
    vLayout5 = new QVBoxLayout;
    vLayout5->addWidget(unit4);
    vLayout5->addWidget(data4);
    vLayout5->setContentsMargins(0,0,0,0);
    vLayout5->setSpacing(0);
    vLayout5->setStretch(1,3);
//    gLayout->addWidget(unit4,0,4,1,1);
//    gLayout->addWidget(data4,1,4,1,1);

    state =  new QComboBox();
    state->setStyleSheet("background-color: rgb(239, 239, 241);selection-color: rgb(0, 0, 0);");
    state->setFont(font);
    state->insertItems(0,list1);
    state->setFrame(true);
    state1 = new QLabel("");
    state1->setStyleSheet("border:0px;");
    state1->setFont(font);
    state1->setAlignment(Qt::AlignCenter);
    vLayout6 = new QVBoxLayout;
    vLayout6->addWidget(state1);
    vLayout6->addWidget(state);
    vLayout6->setContentsMargins(0,0,0,0);
    vLayout6->setSpacing(0);
    vLayout6->setStretch(1,3);
    state->hide();

    hLayout->addLayout(vLayout1,1);
    hLayout->addLayout(vLayout2,2);
    hLayout->addLayout(vLayout3,2);
    hLayout->addLayout(vLayout4,2);
    hLayout->addLayout(vLayout5,2);
    hLayout->addLayout(vLayout6,1);
    hLayout->setContentsMargins(5,5,5,5);    

    hWidget->setLayout(hLayout);
    hWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);}");
    hWidget->show();

    data1->installEventFilter(this);
    data2->installEventFilter(this);
    data3->installEventFilter(this);
    data4->installEventFilter(this);  //安装过滤器

    data5 = NULL;
    data6 = NULL;
    data7 = NULL;
    unit5 = NULL;
    unit6 = NULL;
    unit7 = NULL;

    vLayout7 = NULL;
    vLayout8 = NULL;
    vLayout9 = NULL;
}

void groupItem::singAndSlotConn()
{
    connect(itemName,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
    connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
    connect(itemName,SIGNAL(currentIndexChanged(int)),this,SLOT(itemValueChanged()));
    connect(data1,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    connect(data2,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    connect(data3,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    connect(data4,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
}

void groupItem::commonAddDataItem()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    if(data5 != NULL)
    {
        vLayout7->removeWidget(data5);
        data5->deleteLater();
        data5 = NULL;
    }

    if(data6 != NULL)
    {
        vLayout8->removeWidget(data6);
        data6->deleteLater();
        data6 = NULL;
    }

    if(unit5 != NULL)
    {
        vLayout7->removeWidget(unit5);
        unit5->deleteLater();
        unit5 = NULL;
    }

    if(unit6 != NULL)
    {
        vLayout8->removeWidget(unit6);
        unit6->deleteLater();
        unit6 = NULL;
    }

    if(vLayout7 != NULL)
    {
        hLayout->removeItem(vLayout7);
        vLayout7->deleteLater();
        vLayout7 = NULL;
    }

    if(vLayout8 != NULL)
    {
        hLayout->removeItem(vLayout8);
        vLayout8->deleteLater();
        vLayout8 = NULL;
    }

    if(data7 != NULL)
    {
        vLayout9->removeWidget(data7);
        data7->deleteLater();
        data7 = NULL;
    }

    if(unit7 != NULL)
    {
        vLayout9->removeWidget(unit7);
        unit7->deleteLater();
        unit7 = NULL;
    }

    if(vLayout9 != NULL)
    {
        hLayout->removeItem(vLayout9);
        vLayout9->deleteLater();
        vLayout9 = NULL;
    }

    hLayout->setStretch(0,1);
    hLayout->setStretch(1,2);
    hLayout->setStretch(2,2);
    hLayout->setStretch(3,2);
    hLayout->setStretch(4,2);
    hLayout->setStretch(5,1);
}

void groupItem::powerAddDataItem()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    if(data5 == NULL)
    {
        data5 = new QDoubleSpinBox();
        data5->setStyleSheet("background-color: rgb(239, 239, 241);");
        data5->setFont(font);
        data5->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data5->installEventFilter(this);
        connect(data5,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }
    data5->setMinimum(rangeSets[5].min[3]);
    data5->setMaximum(rangeSets[5].max[3]);
    data5->setDecimals(rangeSets[5].prec[3]);
    data5->setSingleStep(1/pow(10,(double)rangeSets[5].prec[3]));
    data5->setValue(rangeSets[5].minDefault[3]);

    if(data6 == NULL)
    {
        data6 = new QDoubleSpinBox();
        data6->setStyleSheet("background-color: rgb(239, 239, 241);");
        data6->setFont(font);
        data6->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data6->installEventFilter(this);
        connect(data6,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }
    data6->setMinimum(rangeSets[5].min[3]);
    data6->setMaximum(rangeSets[5].max[3]);
    data6->setDecimals(rangeSets[5].prec[3]);
    data6->setSingleStep(1/pow(10,(double)rangeSets[5].prec[3]));
    data6->setValue(rangeSets[5].maxDefault[3]);

    if(data7 == NULL)
    {
        data7 = new QDoubleSpinBox();
        data7->setStyleSheet("background-color: rgb(239, 239, 241);");
        data7->setFont(font);
        data7->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data7->installEventFilter(this);
        connect(data7,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }

    data7->setMinimum(0.5);
    data7->setMaximum(999.9);

    data7->setDecimals(1);
    data7->setSingleStep(0.1);

    if(unit5 == NULL)
    {
        unit5 = new QLabel();
        unit5->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit5->setFont(font);
        unit5->setAlignment(Qt::AlignCenter);
    }
    unit5->setText(tr("电流下限")+QString("(%1)").arg(rangeSets[5].unit[3]));

    if(unit6 == NULL)
    {
        unit6 = new QLabel();
        unit6->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit6->setFont(font);
        unit6->setAlignment(Qt::AlignCenter);
    }
    unit6->setText(tr("电流上限")+QString("(%1)").arg(rangeSets[5].unit[3]));

    if(unit7 == NULL)
    {
        unit7 = new QLabel(tr("延迟时间(S)"));
        unit7->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit7->setFont(font);
        unit7->setAlignment(Qt::AlignCenter);
    }

    if(vLayout7 == NULL)
        vLayout7 = new QVBoxLayout;
    if(vLayout8 == NULL)
        vLayout8 = new QVBoxLayout;
    if(vLayout9 == NULL)
        vLayout9 = new QVBoxLayout;    

    vLayout7->addWidget(unit5);
    vLayout7->addWidget(data5);
    vLayout7->setContentsMargins(0,0,0,0);
    vLayout7->setSpacing(0);
    vLayout7->setStretch(1,3);

    vLayout8->addWidget(unit6);
    vLayout8->addWidget(data6);
    vLayout8->setContentsMargins(0,0,0,0);
    vLayout8->setSpacing(0);
    vLayout8->setStretch(1,3);

    vLayout9->addWidget(unit7);
    vLayout9->addWidget(data7);
    vLayout9->setContentsMargins(0,0,0,0);
    vLayout9->setSpacing(0);
    vLayout9->setStretch(1,3);

    hLayout->insertLayout(2,vLayout7,0);
    hLayout->insertLayout(3,vLayout8,0);
    hLayout->insertLayout(6,vLayout9,0);
    hLayout->setStretch(0,1);
    hLayout->setStretch(1,2);
    hLayout->setStretch(2,2);
    hLayout->setStretch(3,2);
    hLayout->setStretch(4,2);
    hLayout->setStretch(5,2);
    hLayout->setStretch(6,2);
    hLayout->setStretch(7,2);
    hLayout->setStretch(8,1);
}

void groupItem::powerMAddDataItem()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    if(data5 != NULL)
    {
        vLayout7->removeWidget(data5);
        data5->deleteLater();
        data5 = NULL;
    }

    if(data6 != NULL)
    {
        vLayout8->removeWidget(data6);
        data6->deleteLater();
        data6 = NULL;
    }

    if(unit5 != NULL)
    {
        vLayout7->removeWidget(unit5);
        unit5->deleteLater();
        unit5 = NULL;
    }

    if(unit6 != NULL)
    {
        vLayout8->removeWidget(unit6);
        unit6->deleteLater();
        unit6 = NULL;
    }

    if(vLayout7 != NULL)
    {
        hLayout->removeItem(vLayout7);
        vLayout7->deleteLater();
        vLayout7 = NULL;
    }

    if(vLayout8 != NULL)
    {
        hLayout->removeItem(vLayout8);
        vLayout8->deleteLater();
        vLayout8 = NULL;
    }

    if(data7 == NULL)
    {
        data7 = new QDoubleSpinBox();
        data7->setStyleSheet("background-color: rgb(239, 239, 241);");
        data7->setFont(font);
        data7->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data7->installEventFilter(this);
        connect(data7,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }
    data7->setMinimum(0.1);
    data7->setMaximum(9.9);
    data7->setDecimals(1);
    data7->setSingleStep(0.1);

    if(unit7 == NULL)
    {
        unit7 = new QLabel(tr("延迟时间(S)"));
        unit7->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit7->setFont(font);
        unit7->setAlignment(Qt::AlignCenter);
    }

    if(vLayout9 == NULL)
        vLayout9 = new QVBoxLayout;

    vLayout9->addWidget(unit7);
    vLayout9->addWidget(data7);
    vLayout9->setContentsMargins(0,0,0,0);
    vLayout9->setSpacing(0);
    vLayout9->setStretch(1,3);

    hLayout->insertLayout(4,vLayout9,3);
    hLayout->setStretch(0,1);
    hLayout->setStretch(1,2);
    hLayout->setStretch(2,2);
    hLayout->setStretch(3,2);
    hLayout->setStretch(4,2);
    hLayout->setStretch(5,2);
    hLayout->setStretch(6,1);
}

void groupItem::ACAddDataItemXSB()
{
    if(data7 != NULL)
    {
        vLayout9->removeWidget(data7);
        data7->deleteLater();
        data7 = NULL;
    }

    if(unit7 != NULL)
    {
        vLayout9->removeWidget(unit7);
        unit7->deleteLater();
        unit7 = NULL;
    }

    if(vLayout9 != NULL)
    {
        hLayout->removeItem(vLayout9);
        vLayout9->deleteLater();
        vLayout9 = NULL;
    }

    disconnect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
    state->clear();
    QStringList list;
    list<<"1 2"<<"1 3"<<"1 4"<<"2 3"<<"2 4"<<"3 4";
    state->insertItems(0,list);
    state->setCurrentIndex(0);
    state->show();
    connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
}

void groupItem::DCAddDataItemXSB()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    if(data5 != NULL)
    {
        vLayout7->removeWidget(data5);
        data5->deleteLater();
        data5 = NULL;
    }

    if(data6 != NULL)
    {
        vLayout8->removeWidget(data6);
        data6->deleteLater();
        data6 = NULL;
    }

    if(unit5 != NULL)
    {
        vLayout7->removeWidget(unit5);
        unit5->deleteLater();
        unit5 = NULL;
    }

    if(unit6 != NULL)
    {
        vLayout8->removeWidget(unit6);
        unit6->deleteLater();
        unit6 = NULL;
    }

    if(vLayout7 != NULL)
    {
        hLayout->removeItem(vLayout7);
        vLayout7->deleteLater();
        vLayout7 = NULL;
    }

    if(vLayout8 != NULL)
    {
        hLayout->removeItem(vLayout8);
        vLayout8->deleteLater();
        vLayout8 = NULL;
    }

    if(data7 == NULL)
    {
        data7 = new QDoubleSpinBox();
        data7->setStyleSheet("background-color: rgb(239, 239, 241);");
        data7->setFont(font);
        data7->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data7->installEventFilter(this);
        connect(data7,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }
    data7->setMinimum(0.5);
    data7->setMaximum(999.9);
    data7->setDecimals(1);
    data7->setSingleStep(0.1);

    if(unit7 == NULL)
    {
        unit7 = new QLabel(tr("延迟时间(S)"));
        unit7->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit7->setFont(font);
        unit7->setAlignment(Qt::AlignCenter);
    }

    if(vLayout9 == NULL)
        vLayout9 = new QVBoxLayout;

    vLayout9->addWidget(unit7);
    vLayout9->addWidget(data7);
    vLayout9->setContentsMargins(0,0,0,0);
    vLayout9->setSpacing(0);
    vLayout9->setStretch(1,3);

    hLayout->insertLayout(4,vLayout9,3);
    hLayout->setStretch(0,1);
    hLayout->setStretch(1,2);
    hLayout->setStretch(2,2);
    hLayout->setStretch(3,2);
    hLayout->setStretch(4,2);
    hLayout->setStretch(5,2);
    hLayout->setStretch(6,1);

    disconnect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
//    state1->setText(tr("测试顺序"));
    state->clear();
    QStringList list;
    list<<"1 2"<<"1 3"<<"1 4"<<"2 3"<<"2 4"<<"3 4";
    state->insertItems(0,list);
    state->setCurrentIndex(0);
    state->show();
    connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
}

void groupItem::IRAddDataItemXSB()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    if(data5 != NULL)
    {
        vLayout7->removeWidget(data5);
        data5->deleteLater();
        data5 = NULL;
    }

    if(data6 != NULL)
    {
        vLayout8->removeWidget(data6);
        data6->deleteLater();
        data6 = NULL;
    }

    if(unit5 != NULL)
    {
        vLayout7->removeWidget(unit5);
        unit5->deleteLater();
        unit5 = NULL;
    }

    if(unit6 != NULL)
    {
        vLayout8->removeWidget(unit6);
        unit6->deleteLater();
        unit6 = NULL;
    }

    if(vLayout7 != NULL)
    {
        hLayout->removeItem(vLayout7);
        vLayout7->deleteLater();
        vLayout7 = NULL;
    }

    if(vLayout8 != NULL)
    {
        hLayout->removeItem(vLayout8);
        vLayout8->deleteLater();
        vLayout8 = NULL;
    }

    if(data7 == NULL)
    {
        data7 = new QDoubleSpinBox();
        data7->setStyleSheet("background-color: rgb(239, 239, 241);");
        data7->setFont(font);
        data7->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data7->installEventFilter(this);
        connect(data7,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }
    data7->setMinimum(0.5);
    data7->setMaximum(999.9);
    data7->setDecimals(1);
    data7->setSingleStep(0.1);

    if(unit7 == NULL)
    {
        unit7 = new QLabel(tr("延迟时间(S)"));
        unit7->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit7->setFont(font);
        unit7->setAlignment(Qt::AlignCenter);
    }

    if(vLayout9 == NULL)
        vLayout9 = new QVBoxLayout;

    vLayout9->addWidget(unit7);
    vLayout9->addWidget(data7);
    vLayout9->setContentsMargins(0,0,0,0);
    vLayout9->setSpacing(0);
    vLayout9->setStretch(1,3);

    hLayout->insertLayout(4,vLayout9,3);
    hLayout->setStretch(0,1);
    hLayout->setStretch(1,2);
    hLayout->setStretch(2,2);
    hLayout->setStretch(3,2);
    hLayout->setStretch(4,2);
    hLayout->setStretch(5,2);
    hLayout->setStretch(6,1);

    disconnect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
//    state1->setText(tr("测试顺序"));
    state->clear();
    QStringList list;
    list<<"1 2"<<"1 3"<<"1 4"<<"2 3"<<"2 4"<<"3 4";
    state->insertItems(0,list);
    state->setCurrentIndex(0);
    state->show();
    connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
}

void groupItem::IRAddDataItem()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    if(data5 != NULL)
    {
        vLayout7->removeWidget(data5);
        data5->deleteLater();
        data5 = NULL;
    }

    if(data6 != NULL)
    {
        vLayout8->removeWidget(data6);
        data6->deleteLater();
        data6 = NULL;
    }

    if(unit5 != NULL)
    {
        vLayout7->removeWidget(unit5);
        unit5->deleteLater();
        unit5 = NULL;
    }

    if(unit6 != NULL)
    {
        vLayout8->removeWidget(unit6);
        unit6->deleteLater();
        unit6 = NULL;
    }

    if(vLayout7 != NULL)
    {
        hLayout->removeItem(vLayout7);
        vLayout7->deleteLater();
        vLayout7 = NULL;
    }

    if(vLayout8 != NULL)
    {
        hLayout->removeItem(vLayout8);
        vLayout8->deleteLater();
        vLayout8 = NULL;
    }

    if(data7 == NULL)
    {
        data7 = new QDoubleSpinBox();
        data7->setStyleSheet("background-color: rgb(239, 239, 241);");
        data7->setFont(font);
        data7->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data7->installEventFilter(this);
        connect(data7,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }
    data7->setMinimum(0.5);
    data7->setMaximum(999.9);
    data7->setDecimals(1);
    data7->setSingleStep(0.1);

    if(unit7 == NULL)
    {
        unit7 = new QLabel(tr("延迟时间(S)"));
        unit7->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit7->setFont(font);
        unit7->setAlignment(Qt::AlignCenter);
    }

    if(vLayout9 == NULL)
        vLayout9 = new QVBoxLayout;

    vLayout9->addWidget(unit7);
    vLayout9->addWidget(data7);
    vLayout9->setContentsMargins(0,0,0,0);
    vLayout9->setSpacing(0);
    vLayout9->setStretch(1,3);

    hLayout->insertLayout(4,vLayout9,3);
    hLayout->setStretch(0,1);
    hLayout->setStretch(1,2);
    hLayout->setStretch(2,2);
    hLayout->setStretch(3,2);
    hLayout->setStretch(4,2);
    hLayout->setStretch(5,1);
}

void groupItem::startAddDataItem()
{
    QFont font("Microsoft YaHei",12,QFont::Normal);

    if(data5 != NULL)
    {
        vLayout7->removeWidget(data5);
        data5->deleteLater();
        data5 = NULL;
    }

    if(data6 != NULL)
    {
        vLayout8->removeWidget(data6);
        data6->deleteLater();
        data6 = NULL;
    }

    if(unit5 != NULL)
    {
        vLayout7->removeWidget(unit5);
        unit5->deleteLater();
        unit5 = NULL;
    }

    if(unit6 != NULL)
    {
        vLayout8->removeWidget(unit6);
        unit6->deleteLater();
        unit6 = NULL;
    }

    if(vLayout7 != NULL)
    {
        hLayout->removeItem(vLayout7);
        vLayout7->deleteLater();
        vLayout7 = NULL;
    }

    if(vLayout8 != NULL)
    {
        hLayout->removeItem(vLayout8);
        vLayout8->deleteLater();
        vLayout8 = NULL;
    }

    if(data7 == NULL)
    {
        data7 = new QDoubleSpinBox();
        data7->setStyleSheet("background-color: rgb(239, 239, 241);");
        data7->setFont(font);
        data7->setButtonSymbols(QAbstractSpinBox::NoButtons);
        data7->installEventFilter(this);
        connect(data7,SIGNAL(valueChanged(double)),this,SLOT(itemDataChanged()));
    }
    data7->setMinimum(0.5);
    data7->setMaximum(999.9);
    data7->setDecimals(1);
    data7->setSingleStep(0.1);

    if(unit7 == NULL)
    {
        unit7 = new QLabel(tr("延迟时间(S)"));
        unit7->setStyleSheet("color: rgb(239, 239, 241);border:0px;");
        unit7->setFont(font);
        unit7->setAlignment(Qt::AlignCenter);
    }

    if(vLayout9 == NULL)
        vLayout9 = new QVBoxLayout;

    vLayout9->addWidget(unit7);
    vLayout9->addWidget(data7);
    vLayout9->setContentsMargins(0,0,0,0);
    vLayout9->setSpacing(0);
    vLayout9->setStretch(1,3);

    hLayout->insertLayout(4,vLayout9,3);
    hLayout->setStretch(0,1);
    hLayout->setStretch(1,2);
    hLayout->setStretch(2,2);
    hLayout->setStretch(3,2);
    hLayout->setStretch(4,2);
    hLayout->setStretch(5,2);
    hLayout->setStretch(6,1);
}

void groupItem::itemDataChanged()
{
    emit sendChange(0);
}

void groupItem::itemValueChanged()
{    
    emit sendChange(0);
    if(itemName->currentText() == tr("接地") && gnd == 0)
    {
        setItemUnit(getItemUnits(4,0));
        setRange(4);
    }else if(itemName->currentText() == tr("接地") && gnd == 1){
        setItemUnit(getItemUnits(4,1));
        setRange(4);
    }

    if(itemName->currentText() == tr("功率") && tc.machineModel == 1)
    {
        powerAddDataItem();
    }else if(itemName->currentText() == tr("功率") && (tc.machineModel == 2 || tc.machineModel == 5)){
        powerMAddDataItem();
    }else if(itemName->currentText() == tr("绝缘") && tc.machineModel == 1){
        IRAddDataItem();
    }else if(itemName->currentText() == tr("低启") && tc.machineModel == 1){
        startAddDataItem();
    }else if(tc.machineModel == 6 && (itemName->currentText() == tr("交耐") || itemName->currentText() == tr("直耐") || itemName->currentText() == tr("绝缘"))){
        if(itemName->currentText() == tr("交耐"))
        {
            ACAddDataItemXSB();
        }

        if(itemName->currentText() == tr("直耐"))
        {
            DCAddDataItemXSB();
        }

        if(itemName->currentText() == tr("绝缘"))
        {
            IRAddDataItemXSB();
        }
    }else{
        if(tc.machineModel == 3 && (itemName->currentText() == tr("交耐") || itemName->currentText() == tr("直耐") || itemName->currentText() == tr("绝缘")))
        {
            disconnect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
            state->clear();
            QStringList list;
            list<<tr("冷态")<<tr("热态");
            state->insertItems(0,list);
            state->setCurrentIndex(0);
            state->show();
            connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
        }else{
            disconnect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
            state->clear();
            QStringList list;
            if(phase != 2)
            {
                if(tc.phase == 1)
                {
                    list<<"ABC"<<"BC"<<"AC"<<"AB";
                }else if(tc.phase == 2){
                    list<<"ABC"<<"BC"<<"AC"<<"AB"<<"A"<<"B"<<"C";
                }else{
                    list<<tr("静态")<<tr("动态");
                }
            }else{
                list<<tr("静态")<<tr("动态");
            }
            state->insertItems(0,list);
            state->setCurrentIndex(0);
            state->show();
            connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
        }
        commonAddDataItem();
    }
}

void groupItem::dataSetFocus()
{
    data1->hasFocus();
}

bool groupItem::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == data1)
    {
        if(event->type()==QEvent::FocusIn)
        {
            activeRow = rowNum;
            emit sendData(activeRow);

            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
        }
        else if(event->type()==QEvent::FocusOut)
        {
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
        }
    }

    if(watched == data2)
    {
        if(event->type()==QEvent::FocusIn)
        {
            activeRow = rowNum;
            emit sendData(activeRow);
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
        }
        else if(event->type()==QEvent::FocusOut)
        {
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
        }
    }

    if(watched == data3)
    {
        if(event->type()==QEvent::FocusIn)
        {
            activeRow = rowNum;
            emit sendData(activeRow);
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
        }
        else if(event->type()==QEvent::FocusOut)
        {
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
        }
    }

    if(watched == data4)
    {
        if(event->type()==QEvent::FocusIn)
        {
            activeRow = rowNum;
            emit sendData(activeRow);
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
        }
        else if(event->type()==QEvent::FocusOut)
        {
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
        }
    }

    if(watched == data5)
    {
        if(event->type()==QEvent::FocusIn)
        {
            activeRow = rowNum;
            emit sendData(activeRow);
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
        }
        else if(event->type()==QEvent::FocusOut)
        {
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
        }
    }

    if(watched == data6)
    {
        if(event->type()==QEvent::FocusIn)
        {
            activeRow = rowNum;
            emit sendData(activeRow);
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
        }
        else if(event->type()==QEvent::FocusOut)
        {
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
        }
    }

    if(watched == data7)
    {
        if(event->type()==QEvent::FocusIn)
        {
            activeRow = rowNum;
            emit sendData(activeRow);
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
        }
        else if(event->type()==QEvent::FocusOut)
        {
            hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
        }
    }

    return QWidget::eventFilter(watched,event);
}

void groupItem::setCurrentRowNumber()
{
    activeRow = rowNum;
}

void groupItem::setRowNumber(int row)
{
    rowNum = row;
}

void groupItem::setGNDShow(int value)
{
    gnd = value;
}

void groupItem::setBackgroundColor(int index)
{
    if(index == 1)
        hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
    else
        hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
}

void groupItem::setItemName(QString name)
{
    itemName->setCurrentText(name);

    if(name == tr("功率") && tc.machineModel == 1)
    {
        powerAddDataItem();
    }else if(name == tr("功率") && (tc.machineModel == 2 || tc.machineModel == 5)){
        powerMAddDataItem();
    }else if(name == tr("绝缘") && tc.machineModel == 1){
        IRAddDataItem();
    }else if(tc.machineModel == 6 && (name == tr("交耐") || name == tr("直耐") || name == tr("绝缘"))){
        if(itemName->currentText() == tr("交耐"))
        {
            ACAddDataItemXSB();
        }

        if(itemName->currentText() == tr("直耐"))
        {
            DCAddDataItemXSB();
        }

        if(itemName->currentText() == tr("绝缘"))
        {
            IRAddDataItemXSB();
        }
    }else if(name == tr("低启") && tc.machineModel == 1){
        startAddDataItem();
    }
}

void groupItem::setItemUnit(QStringList units)
{
    unit1->setText(units.at(0));
    unit2->setText(units.at(1));
    unit3->setText(units.at(2));
    unit4->setText(units.at(3));

    if(itemName->currentText() == tr("直耐") && tc.machineModel == 6)
    {
        unit2->setText(tr("电压降落(V)"));
    }
}

void groupItem::itemConfigRead(groupSetItem *it)
{
    int index;
    if(itemName->currentText() == tr("交耐"))
        index = 1;
    else  if(itemName->currentText() == tr("直耐"))
        index = 2;
    else  if(itemName->currentText() == tr("绝缘"))
        index = 3;
    else  if(itemName->currentText() == tr("接地"))
        index = 4;
    else  if(itemName->currentText() == tr("泄漏"))
        index = 5;
    else  if(itemName->currentText() == tr("功率"))
        index = 6;
    else  if(itemName->currentText() == tr("低启"))
        index = 7;
    else  if(itemName->currentText() == tr("开短"))
        index = 8;

    it->id = index;
    if(it->id == 0)
        return;
    it->itemName = itemName->currentText();
    it->data[0] = data1->value();
    it->data[1] = data2->value();
    it->data[2] = data3->value();
    it->data[3] = data4->value();

    if(tc.machineModel == 3)
    {
        if(it->id == 1 || it->id == 2 || it->id == 3 || it->id == 5)
            it->state = state->currentIndex();
        else
            it->state = -1;
    }else if(tc.machineModel == 6){
        if(it->id == 1 || it->id == 2 || it->id == 3)
            it->state = state->currentIndex();
        else
            it->state = -1;
    }else{
        if(it->id == 5)
            it->state = state->currentIndex();
        else
            it->state = -1;
    }

    int i = it->id-1;

    it->prec[0] = data1->decimals();
    it->prec[1] = data2->decimals();
    it->prec[2] = data3->decimals();
    it->prec[3] = data4->decimals();

    if(it->id == 4)
        it->unit[0] = rangeSets[i].unit[3];
    else
        it->unit[0] = rangeSets[i].unit[0];
    it->unit[1] = rangeSets[i].unit[1];
    it->unit[2] = rangeSets[i].unit[1];
    it->unit[3] = rangeSets[i].unit[2];

    if(data5 != NULL)
    {
        it->data[4] = data5->value();
        it->prec[4] = data5->decimals();
        it->unit[4] = rangeSets[i].unit[3];
        data5->setMinimum(rangeSets[i].min[4]);
        data5->setMaximum(rangeSets[i].max[4]);
        data5->setDecimals(rangeSets[i].prec[4]);
        data5->setSingleStep(1/pow(10,(double)rangeSets[i].prec[4]));
    }

    if(data6 != NULL)
    {
        it->data[5] = data6->value();
        it->prec[5] = data6->decimals();
        it->unit[5] = rangeSets[i].unit[3];
        data6->setMinimum(rangeSets[i].min[5]);
        data6->setMaximum(rangeSets[i].max[5]);
        data6->setDecimals(rangeSets[i].prec[5]);
        data6->setSingleStep(1/pow(10,(double)rangeSets[i].prec[5]));
    }

    if(data7 != NULL)
    {
        it->data[6] = data7->value();
        it->prec[6] = 1;
        it->unit[6] = "S";
    }
}

void groupItem::setTestData(double data[])
{    
    data1->setValue(data[0]);
    data2->setValue(data[1]);
    data3->setValue(data[2]);
    data4->setValue(data[3]);

    if(data5 != NULL)
        data5->setValue(data[4]);
    if(data6 != NULL)
        data6->setValue(data[5]);
    if(data7 != NULL)
        data7->setValue(data[6]);
}

/****************************************
 * 方法名：getItemUnits
 * 说明：
****************************************/
QStringList groupItem::getItemUnits(int index,int gnd)
{
    QStringList list;
    switch(index)
    {
        case 1:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[0].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[0].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[0].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[0].unit[2]);
            break;
        case 2:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[1].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[1].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[1].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[1].unit[2]);
            break;
        case 3:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[2].unit[0])<<tr("电阻下限")+QString("(%1)").arg(rangeSets[2].unit[1])<<tr("电阻上限")+QString("(%1)").arg(rangeSets[2].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[2].unit[2]);
            break;
        case 4:
            if(gnd == 0)
                list<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电阻下限")+QString("(%1)").arg(rangeSets[3].unit[1])<<tr("电阻上限")+QString("(%1)").arg(rangeSets[3].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
            else
                list<<tr("电流")+QString("(%1)").arg(rangeSets[3].unit[3])<<tr("电压下限")+QString("(%1)").arg(rangeSets[3].unit[0])<<tr("电压上限")+QString("(%1)").arg(rangeSets[3].unit[0])<<tr("时间")+QString("(%1)").arg(rangeSets[3].unit[2]);
            break;
        case 5:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[4].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[4].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[4].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[4].unit[2]);
            break;
        case 6:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[5].unit[0])<<tr("功率下限")+QString("(%1)").arg(rangeSets[5].unit[1])<<tr("功率上限")+QString("(%1)").arg(rangeSets[5].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[5].unit[2]);
            break;
        case 7:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[6].unit[0])<<tr("电流下限")+QString("(%1)").arg(rangeSets[6].unit[1])<<tr("电流上限")+QString("(%1)").arg(rangeSets[6].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[6].unit[2]);
            break;
        case 8:
            list<<tr("电压")+QString("(%1)").arg(rangeSets[7].unit[0])<<tr("电阻下限")+QString("(%1)").arg(rangeSets[7].unit[1])<<tr("电阻上限")+QString("(%1)").arg(rangeSets[7].unit[1])<<tr("时间")+QString("(%1)").arg(rangeSets[7].unit[2]);
            break;
        default:
            list<<tr("电压(V)")<<tr("电流下限(mA)")<<tr("电流上限(mA)")<<tr("时间(S)");
            break;
    }
    setItemUnit(list);
    return list;
}

void groupItem::setItemSize(int w,int h)
{
    hWidget->resize(w,h);
}

void groupItem::setRange(int itemId)
{    
    disconnect(itemName,SIGNAL(currentIndexChanged(int)),this,SLOT(checkData(int)));
    int index = itemId-1;
    int gnd;

    if(unit2->text().mid(0,2) == tr("电阻"))
        gnd = 0;
    else
        gnd = 1;

    if(itemId == 5)
    {
        state->setCurrentIndex(0);
        state->show();
    }else if((itemId == 1 || itemId == 2 || itemId == 3) && tc.machineModel == 3){
        disconnect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
        state->clear();
        QStringList list;
        list<<tr("冷态")<<tr("热态");
        state->insertItems(0,list);
        state->setCurrentIndex(0);
        state->show();
        connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
    }else if((itemId == 1 || itemId == 2 || itemId == 3) && tc.machineModel == 6){
        disconnect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
        state->clear();
        QStringList list;
        list<<"1 2"<<"1 3"<<"1 4"<<"2 3"<<"2 4"<<"3 4";
        state->insertItems(0,list);
        state->setCurrentIndex(0);
        state->show();
        connect(state,SIGNAL(activated(QString)),this,SLOT(setCurrentRowNumber()));
    }else{
        state->hide();
    }

    if(itemId == 0)
    {
        data1->hide();
        data2->hide();
        data3->hide();
        data4->hide();
        unit1->hide();
        unit2->hide();
        unit3->hide();
        unit4->hide();
    }else{
        data1->show();
        data2->show();
        data3->show();
        data4->show();
        unit1->show();
        unit2->show();
        unit3->show();
        unit4->show();
    }

    if(itemId > 0 && itemId < 6 && itemId != 4)
    {
        data1->setMinimum(rangeSets[index].min[0]);
        data1->setMaximum(rangeSets[index].max[0]);
        data1->setDecimals(rangeSets[index].prec[0]);
        data1->setSingleStep(1/pow(10,(double)rangeSets[index].prec[0]));

        if(itemId == 2)
        {
            if(tc.machineModel == 6)
            {
                data2->setMinimum(200.0);
                data2->setMaximum(8000.0);
                data2->setDecimals(1);
                data2->setSingleStep(0.1);
            }else{
                data2->setMinimum(0.0);
                data2->setMaximum(1.0);
                data2->setDecimals(rangeSets[index].prec[1]);
                data2->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));
            }
        }else if(itemId == 5){
            data2->setMinimum(0.0);
            data2->setMaximum(5.0);
            data2->setDecimals(rangeSets[index].prec[1]);
            data2->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));
        }else{
            data2->setMinimum(rangeSets[index].min[1]);
            data2->setMaximum(rangeSets[index].max[1]);
            data2->setDecimals(rangeSets[index].prec[1]);
            data2->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));
        }

        if(itemId == 3)
        {
            data3->setMinimum(0.0);
        }else{
            data3->setMinimum(rangeSets[index].min[1]);
        }
        data3->setMaximum(rangeSets[index].max[1]);
        data3->setDecimals(rangeSets[index].prec[1]);
        data3->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));

        data4->setMinimum(rangeSets[index].min[2]);
        data4->setMaximum(rangeSets[index].max[2]);
        data4->setDecimals(rangeSets[index].prec[2]);
        data4->setSingleStep(1/pow(10,(double)rangeSets[index].prec[2]));
    }else if(itemId == 4){
        data1->setMinimum(rangeSets[index].min[3]);
        data1->setMaximum(rangeSets[index].max[3]);
        data1->setDecimals(rangeSets[index].prec[3]);
        data1->setSingleStep(1/pow(10,(double)rangeSets[index].prec[3]));

        int a;
        if(gnd == 0)
            a = 1;
        else
            a = 0;

        data2->setMinimum(rangeSets[index].min[a]);
        data2->setMaximum(rangeSets[index].max[a]);
        data2->setDecimals(rangeSets[index].prec[a]);
        data2->setSingleStep(1/pow(10,(double)rangeSets[index].prec[a]));

        data3->setMinimum(rangeSets[index].min[a]);
        data3->setMaximum(rangeSets[index].max[a]);
        data3->setDecimals(rangeSets[index].prec[a]);
        data3->setSingleStep(1/pow(10,(double)rangeSets[index].prec[a]));

        data4->setMinimum(rangeSets[index].min[2]);
        data4->setMaximum(rangeSets[index].max[2]);
        data4->setDecimals(rangeSets[index].prec[2]);
        data4->setSingleStep(1/pow(10,(double)rangeSets[index].prec[2]));
    }else if(itemId == 6){
        data1->setMinimum(rangeSets[index].min[0]);
        data1->setMaximum(rangeSets[index].max[0]);
        data1->setDecimals(rangeSets[index].prec[0]);
        data1->setSingleStep(1/pow(10,(double)rangeSets[index].prec[0]));

        data2->setMinimum(rangeSets[index].min[1]);
        data2->setMaximum(rangeSets[index].max[1]);
        data2->setDecimals(rangeSets[index].prec[1]);
        data2->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));

        data3->setMinimum(rangeSets[index].min[1]);
        data3->setMaximum(rangeSets[index].max[1]);
        data3->setDecimals(rangeSets[index].prec[1]);
        data3->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));

        data4->setMinimum(rangeSets[index].min[2]);
        data4->setMaximum(rangeSets[index].max[2]);
        data4->setDecimals(rangeSets[index].prec[2]);
        data4->setSingleStep(1/pow(10,(double)rangeSets[index].prec[2]));
    }else if(itemId == 7){
        data1->setMinimum(rangeSets[index].min[0]);
        data1->setMaximum(rangeSets[index].max[0]);
        data1->setDecimals(rangeSets[index].prec[0]);
        data1->setSingleStep(1/pow(10,(double)rangeSets[index].prec[0]));

        data2->setMinimum(rangeSets[index].min[1]);
        data2->setMaximum(rangeSets[index].max[1]);
        data2->setDecimals(rangeSets[index].prec[1]);
        data2->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));

        data3->setMinimum(rangeSets[index].min[1]);
        data3->setMaximum(rangeSets[index].max[1]);
        data3->setDecimals(rangeSets[index].prec[1]);
        data3->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));

        data4->setMinimum(rangeSets[index].min[2]);
        data4->setMaximum(rangeSets[index].max[2]);
        data4->setDecimals(rangeSets[index].prec[2]);
        data4->setSingleStep(1/pow(10,(double)rangeSets[index].prec[2]));
    }else if(itemId == 8){
        data1->setMinimum(rangeSets[index].min[0]);
        data1->setMaximum(rangeSets[index].max[0]);
        data1->setDecimals(rangeSets[index].prec[0]);
        data1->setSingleStep(1/pow(10,(double)rangeSets[index].prec[0]));

        data2->setMinimum(rangeSets[index].min[1]);
        data2->setMaximum(rangeSets[index].max[1]);
        data2->setDecimals(rangeSets[index].prec[1]);
        data2->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));

        data3->setMinimum(rangeSets[index].min[1]);
        data3->setMaximum(rangeSets[index].max[1]);
        data3->setDecimals(rangeSets[index].prec[1]);
        data3->setSingleStep(1/pow(10,(double)rangeSets[index].prec[1]));

        data4->setMinimum(rangeSets[index].min[2]);
        data4->setMaximum(rangeSets[index].max[2]);
        data4->setDecimals(rangeSets[index].prec[2]);
        data4->setSingleStep(1/pow(10,(double)rangeSets[index].prec[2]));
    }

    connect(itemName,SIGNAL(currentIndexChanged(QString)),this,SLOT(checkData(QString)));
}

void groupItem::checkData(QString itemName)
{
    int index;
    if(itemName == tr("交耐"))
        index = 1;
    else  if(itemName == tr("直耐"))
        index = 2;
    else  if(itemName == tr("绝缘"))
        index = 3;
    else  if(itemName == tr("接地"))
        index = 4;
    else  if(itemName == tr("泄漏"))
        index = 5;
    else  if(itemName == tr("功率"))
        index = 6;
    else  if(itemName == tr("低启"))
        index = 7;
    else  if(itemName == tr("开短"))
        index = 8;

    setRange(index);
    getItemUnits(index,gnd);
    emit sendData(-1);
    if(index > 0 && index < 9)
    {
        if(index == 4)
            data1->setValue(rangeSets[index-1].def[3]);
        else
            data1->setValue(rangeSets[index-1].def[0]);
        data2->setValue(rangeSets[index-1].minDefault[1]);
        data3->setValue(rangeSets[index-1].maxDefault[1]);
        data4->setValue(rangeSets[index-1].def[2]);
    }
}

void groupItem::setItemPhase(int value)
{
    phase = value;
}

void groupItem::setItemStateST(int value,QString name)
{
    phase = value;
    if(name == tr("泄漏"))
    {
        state->clear();
        QStringList list;
        if(value == 2)
        {
            list<<tr("静态")<<tr("动态");
        }else{
            if(tc.phase == 1)
            {
                list<<"ABC"<<"BC"<<"AC"<<"AB";
            }else if(tc.phase == 2){
                list<<"ABC"<<"BC"<<"AC"<<"AB"<<"A"<<"B"<<"C";
            }
        }
        state->insertItems(0,list);
        state->setCurrentIndex(0);
        state->show();
    }
}

void groupItem::setItemStateHide(int value)
{
    if(value == 1)
        state->setVisible(true);
    else
        state->setVisible(false);
}

void groupItem::setItemState(int index)
{
    if(tc.phase == 1 || tc.phase == 2)
    {
        state->setCurrentIndex(index);
    }else if(tc.machineModel == 6){
        if(index < 0 || index > 5)
            state->setCurrentIndex(0);
        else
            state->setCurrentIndex(index);
    }else{
        if(index < 0 || index > 1)
            state->setCurrentIndex(0);
        else
            state->setCurrentIndex(index);
    }
}
