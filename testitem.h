#ifndef TESTITEM_H
#define TESTITEM_H

#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QPalette>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "loginsetinfo.h"

class testItem : public QWidget
{
    Q_OBJECT
public:
    explicit testItem(QWidget *parent = 0);
    void setItemName(QString name);
    void setItemSize(int w,int h);
    void setItemUnit(QStringList units);
    void setTestData(int flag,QStringList data);
    void setItemPix(int index);
    void setBackgroundColor(int index);
    void setCurrentMax(float value);
    void setLLTData2(QString data);
    void showLLTData2(int prec);
    QList<testDataStr> testDataList;

private:
    QWidget *hWidget;
    QGridLayout *gLayout;
    QHBoxLayout *hLayout;
    QVBoxLayout *vLayout1;
    QVBoxLayout *vLayout2;
    QVBoxLayout *vLayout3;
    QVBoxLayout *vLayout4;
    QVBoxLayout *vLayout5;
    QVBoxLayout *vLayout6;

    QLabel *itemName;
    QLabel *itemName1;
    QLabel *data1;
    QLabel *unit1;
    QLabel *data2;
    QLabel *unit2;
    QLabel *data3;
    QLabel *unit3;
    QLabel *data4;
    QLabel *unit4;
    QLabel *state;
    QLabel *state1;
    float currentMax;
    float currentMax1;

signals:

public slots:
};

#endif // TESTITEM_H
