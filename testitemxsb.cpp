#include "testitemxsb.h"

testItemXSB::testItemXSB(QWidget *parent) : QWidget(parent)
{
    QPalette pa;
    pa.setColor(QPalette::Text,Qt::white);

    QFont font("Microsoft YaHei",40,QFont::Normal);
    QFont font1("Microsoft YaHei",32,QFont::Normal);
    hWidget = new QWidget(this);
//    gLayout = new QGridLayout(this);

    entrance = new QLabel(tr("入:"));
    out = new QLabel(tr("出:"));
    purify = new QLabel(tr("净化效率:"));
    entranceData = new QLabel("");
    outData = new QLabel("");
    purifyData = new QLabel("");

    entrance->setPalette(pa);
    entrance->setFont(font1);
    entrance->setAlignment(Qt::AlignCenter);

    out->setPalette(pa);
    out->setFont(font1);
    out->setAlignment(Qt::AlignCenter);

    purify->setPalette(pa);
    purify->setFont(font1);
    purify->setAlignment(Qt::AlignCenter);

    entranceData->setPalette(pa);
    entranceData->setFont(font);
    entranceData->setAlignment(Qt::AlignCenter);

    outData->setPalette(pa);
    outData->setFont(font);
    outData->setAlignment(Qt::AlignCenter);

    purifyData->setPalette(pa);
    purifyData->setFont(font);
    purifyData->setAlignment(Qt::AlignCenter);

    hLayout1 = new QHBoxLayout;
    hLayout1->addWidget(entrance);
    hLayout1->addWidget(entranceData);
    hLayout1->setSpacing(0);

    hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(out);
    hLayout2->addWidget(outData);
    hLayout2->setSpacing(0);

    hLayout3 = new QHBoxLayout;
    hLayout3->addWidget(purify);
    hLayout3->addWidget(purifyData);
    hLayout3->setSpacing(0);

    hLayout4 = new QHBoxLayout;
    hLayout4->addLayout(hLayout1,0);
    hLayout4->addLayout(hLayout2,0);
    hLayout4->addLayout(hLayout3,0);
    hLayout4->setContentsMargins(1,1,1,1);
    hLayout4->setSpacing(30);

    hWidget->setLayout(hLayout4);
    hWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    hWidget->setStyleSheet("QWidget{border:0px outset rgb(239, 239, 241);}QLabel{border:0px;}");

    hWidget->show();
}

void testItemXSB::setTestData1(QString data)
{
    entranceData->setText(data);
}

void testItemXSB::setTestData2(QString data)
{
    outData->setText(data);
}

void testItemXSB::setTestData3(QString data)
{
    purifyData->setText(data);
}

void testItemXSB::setItemPix(int index)
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
    purify->setPalette(pa);
    purifyData->setPalette(pa);
}

void testItemXSB::setItemSize(int w,int h)
{
    hWidget->resize(w,h);
}

void testItemXSB::setBackgroundColor(int index)
{
    if(index == 1)
        hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(19,149,235);}");
    else
        hWidget->setStyleSheet("QWidget{border:0.5px outset rgb(239, 239, 241);background-color: rgb(11,63,158);}");
}
