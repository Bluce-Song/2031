#include "testitem.h"

testItem::testItem(QWidget *parent) : QWidget(parent)
{
    currentMax = 0.0;
    QPixmap *pix;
    pix = new QPixmap(":/resource/灰白.png");
    QPalette pa;
    pa.setColor(QPalette::Text,Qt::white);

    QFont font("Microsoft YaHei",24,QFont::Bold);
    QFont font1("Microsoft YaHei",32,QFont::Normal);
    QFont font2("Microsoft YaHei",18,QFont::Normal);
    hWidget = new QWidget(this);
//    gLayout = new QGridLayout(this);
    hLayout = new QHBoxLayout(this);

    itemName = new QLabel(tr("电压(V)"));
    itemName1 = new QLabel(tr("交耐"));
    itemName->setPalette(pa);
    itemName->setFont(font2);
    itemName->setAlignment(Qt::AlignCenter);
    itemName1->setPalette(pa);
    itemName1->setFont(font1);
    itemName1->setAlignment(Qt::AlignCenter);
    vLayout1 = new QVBoxLayout;
    vLayout1->addWidget(itemName);
    vLayout1->addWidget(itemName1);
    vLayout1->setContentsMargins(0,0,0,0);
    vLayout1->setSpacing(0);
    vLayout1->setStretch(3,5);
    itemName->hide();
//    gLayout->addWidget(itemName,0,0,1,1);
//    gLayout->addWidget(itemName1,1,0,1,1);   

    unit1 = new QLabel(tr("电压(V)"));
    data1 = new QLabel("0.0");
    data1->setPalette(pa);
    data1->setFont(font1);
    data1->setAlignment(Qt::AlignCenter);
    unit1->setPalette(pa);
    unit1->setFont(font2);
    unit1->setAlignment(Qt::AlignCenter);
    vLayout2 = new QVBoxLayout;
    vLayout2->addWidget(unit1);
    vLayout2->addWidget(data1);
    vLayout2->setContentsMargins(0,0,0,0);
    vLayout2->setSpacing(0);
    vLayout2->setStretch(3,5);
//    gLayout->addWidget(unit1,0,1,1,1);
//    gLayout->addWidget(data1,1,1,1,1);

    unit2 = new QLabel(tr("电流(mA)"));
    data2 = new QLabel("0.0");
    data2->setPalette(pa);
    data2->setFont(font1);
    data2->setAlignment(Qt::AlignCenter);
    unit2->setPalette(pa);
    unit2->setFont(font2);
    unit2->setAlignment(Qt::AlignCenter);
    vLayout3 = new QVBoxLayout;
    vLayout3->addWidget(unit2);
    vLayout3->addWidget(data2);
    vLayout3->setContentsMargins(0,0,0,0);
    vLayout3->setSpacing(0);
    vLayout3->setStretch(3,5);
//    gLayout->addWidget(unit2,0,2,1,1);
//    gLayout->addWidget(data2,1,2,1,1);

    unit3 = new QLabel("");
    data3 = new QLabel("");
    data3->setPalette(pa);
    data3->setFont(font1);
    data3->setAlignment(Qt::AlignCenter);
    unit3->setPalette(pa);
    unit3->setFont(font2);
    unit3->setAlignment(Qt::AlignCenter);
    vLayout4 = new QVBoxLayout;
    vLayout4->addWidget(unit3);
    vLayout4->addWidget(data3);
    vLayout4->setContentsMargins(0,0,0,0);
    vLayout4->setSpacing(0);
    vLayout4->setStretch(3,5);
//    gLayout->addWidget(unit3,0,3,1,1);
//    gLayout->addWidget(data3,1,3,1,1);

    unit4 = new QLabel(tr("时间(S)"));
    data4 = new QLabel("0.0");
    data4->setPalette(pa);
    data4->setFont(font1);
    data4->setAlignment(Qt::AlignCenter);
    unit4->setPalette(pa);
    unit4->setFont(font2);
    unit4->setAlignment(Qt::AlignCenter);
    vLayout5 = new QVBoxLayout;
    vLayout5->addWidget(unit4);
    vLayout5->addWidget(data4);
    vLayout5->setContentsMargins(0,0,0,0);
    vLayout5->setSpacing(0);
    vLayout5->setStretch(3,5);
//    gLayout->addWidget(unit4,0,4,1,1);
//    gLayout->addWidget(data4,1,4,1,1);

//    state = new QLabel("");
//    state1 = new QLabel("");
//    state->setStyleSheet("color: rgb(239, 239, 241);");
//    state->setFont(font2);
//    state->setAlignment(Qt::AlignCenter);
//    state1->setStyleSheet("QLabel{color: rgb(239, 239, 241);}");
//    state1->setFont(font1);
//    state1->setPixmap(*pix);
//    state1->setAlignment(Qt::AlignCenter);
//    vLayout6 = new QVBoxLayout;
//    vLayout6->addWidget(state);
//    vLayout6->addWidget(state1);
//    vLayout6->setContentsMargins(0,0,0,0);
//    vLayout6->setSpacing(0);
//    vLayout6->setStretch(3,5);
//    state->hide();
//    gLayout->addWidget(state,0,5,1,1);
//    gLayout->addWidget(state1,1,5,1,1);

//    gLayout->setVerticalSpacing(0);
//    gLayout->setHorizontalSpacing(0);
//    gLayout->setRowStretch(3,5);
//    gLayout->setColumnStretch(0,2);
//    gLayout->setColumnStretch(1,3);
//    gLayout->setColumnStretch(2,3);
//    gLayout->setColumnStretch(3,3);
//    gLayout->setColumnStretch(4,3);
//    gLayout->setColumnStretch(5,1);
//    gLayout->setContentsMargins(1,1,1,1);

    hLayout->addLayout(vLayout1,2);
    hLayout->addLayout(vLayout2,3);
    hLayout->addLayout(vLayout3,3);
    hLayout->addLayout(vLayout4,3);
    hLayout->addLayout(vLayout5,3);
//    hLayout->addLayout(vLayout6,1);
    hLayout->setContentsMargins(1,1,1,1);

    hWidget->setLayout(hLayout);
    hWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);}QLabel{border:0px;}");
//    hWidget->setStyleSheet("border:0.5px outset rgb(239, 239, 241);");
//    hWidget->resize(1000,100);
    hWidget->show();
}

void testItem::setItemName(QString name)
{
    itemName1->setText(name);
}

void testItem::setItemUnit(QStringList units)
{    
    unit1->setText(units.at(0));
    unit2->setText(units.at(1));
    unit3->setText(units.at(2));
    unit4->setText(units.at(3));
}

void testItem::setCurrentMax(float value)
{
    currentMax = value;
}

void testItem::setLLTData2(QString data)
{
    data2->setText(data);
}

void testItem::showLLTData2(int prec)
{
    if(itemName1->text() == tr("泄漏"))
        data2->setText(QString::number(currentMax>currentMax1?currentMax:currentMax1,10,prec));
//    qDebug()<<QString::number(currentMax,10,prec);
}

void testItem::setTestData(int flag,QStringList data)
{
    data1->setText(data.at(0));
    data2->setText(data.at(1));
    data3->setText(data.at(2));
    data4->setText(data.at(3));

    QString num = data.at(1);
    float current = num.toFloat();

    if(flag == 1)
        currentMax = current;
    else
        currentMax1 = current;
//    data.clear();
}

void testItem::setItemPix(int index)
{
    QPalette pa;

    switch(index)
    {
        case 0:     //待测
            pa.setColor(QPalette::Text,Qt::white);
            break;
        case 1:     //测试中
            pa.setColor(QPalette::Text,Qt::yellow);
            break;
        case 2:     //合格
            pa.setColor(QPalette::Text,Qt::green);
            break;
        case 3:     //不合格
            pa.setColor(QPalette::Text,Qt::red);
            break;
    }
    itemName->setPalette(pa);
    itemName1->setPalette(pa);
    data1->setPalette(pa);
    unit1->setPalette(pa);
    data2->setPalette(pa);
    unit2->setPalette(pa);
    data3->setPalette(pa);
    unit3->setPalette(pa);
    data4->setPalette(pa);
    unit4->setPalette(pa);
}

void testItem::setItemSize(int w,int h)
{
    hWidget->resize(w,h);
}

void testItem::setBackgroundColor(int index)
{
    if(index == 1)
        hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
    else
        hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
}
