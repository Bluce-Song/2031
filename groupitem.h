#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <math.h>
#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QEvent>
#include <QPalette>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "loginsetinfo.h"
#include "choosegroup.h"

extern testConfig tc;
extern range_Item rangeSets[8];
extern int activeRow;

class groupItem : public QWidget
{
    Q_OBJECT
public:
    explicit groupItem(QWidget *parent = 0);

    int rowNum;
    int gnd;
    void setItemName(QString name);
    void setItemSize(int w,int h);
    void setItemUnit(QStringList units);
    void setTestData(double data[]);
    void setRange(int itemId);
    void setItemState(int index);
    void setItemPhase(int value);
    void setItemStateHide(int value);
    void setItemStateST(int value,QString name);
    void setRowNumber(int row);
    void setBackgroundColor(int index);
    void itemConfigRead(groupSetItem *it);
    void dataSetFocus();
    void setGNDShow(int value);
    void singAndSlotConn();

    QComboBox *itemName;

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
    QVBoxLayout *vLayout7;
    QVBoxLayout *vLayout8;
    QVBoxLayout *vLayout9;

    QLabel *itemName1;

    QDoubleSpinBox *data1;
    QLabel *unit1;

    QDoubleSpinBox *data2;
    QLabel *unit2;

    QDoubleSpinBox *data3;
    QLabel *unit3;

    QDoubleSpinBox *data4;
    QLabel *unit4;

    QDoubleSpinBox *data5;
    QLabel *unit5;

    QDoubleSpinBox *data6;
    QLabel *unit6;

    QDoubleSpinBox *data7;
    QLabel *unit7;

    QComboBox *state;
    QLabel *state1;

    int phase;

    QStringList getItemUnits(int index,int gnd);
    void powerAddDataItem();
    void powerMAddDataItem();
    void IRAddDataItem();
    void IRAddDataItemXSB();
    void ACAddDataItemXSB();
    void DCAddDataItemXSB();
    void startAddDataItem();
    void commonAddDataItem();
signals:
    QString sendData(int);
    QString sendChange(int);

public slots:
    void checkData(QString itemName);
    void itemValueChanged();
    void itemDataChanged();
    void setCurrentRowNumber();    
    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // GROUPITEM_H
