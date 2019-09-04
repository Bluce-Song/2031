#ifndef MVTESTUI_H
#define MVTESTUI_H

#include <QWidget>
#include <QDebug>
#include <QListView>
#include <QresizeEvent>
#include <QStandardItemModel>
#include "testitem.h"

namespace Ui {
class mvTestUI;
}

class mvTestUI : public QWidget
{
    Q_OBJECT

public:
    explicit mvTestUI(QWidget *parent = 0);
    ~mvTestUI();
    void setupModel();
    void setupView();
    void modelViewFun();

private:
    Ui::mvTestUI *ui;

    int widgetWidth,widgetHeight,canDo;
    testItem *ti;
    QStandardItem *item;
    QStandardItemModel *model;

private slots:
    void setNewSize();
    void setNewSize1();
    void resizeEvent(QResizeEvent *);
};

#endif // MVTESTUI_H
