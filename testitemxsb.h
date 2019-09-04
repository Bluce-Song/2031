#ifndef TESTITEMXSB_H
#define TESTITEMXSB_H

#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QPalette>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "loginsetinfo.h"

class testItemXSB : public QWidget
{
    Q_OBJECT
public:
    explicit testItemXSB(QWidget *parent = 0);

    void setTestData1(QString data);
    void setTestData2(QString data);
    void setTestData3(QString data);
    void setItemPix(int index);
    void setItemSize(int w,int h);
    void setBackgroundColor(int index);

private:
    QWidget *hWidget;
    QHBoxLayout *hLayout1;
    QHBoxLayout *hLayout2;
    QHBoxLayout *hLayout3;
    QHBoxLayout *hLayout4;

    QLabel *entrance;
    QLabel *out;
    QLabel *purify;
    QLabel *entranceData;
    QLabel *outData;
    QLabel *purifyData;

signals:

public slots:
};

#endif // TESTITEMXSB_H
