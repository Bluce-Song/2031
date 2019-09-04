#ifndef TEST_NEW_H
#define TEST_NEW_H

#include <QMainWindow>
#include <QLabel>
#include <QFrame>
#include <QToolButton>
#include <QToolBar>
#include <QMenu>
#include <QMenuBar>
#include <QTextBrowser>

namespace Ui {
class test_new;
}

class test_new : public QMainWindow
{
    Q_OBJECT

public:
    explicit test_new(QWidget *parent = 0);
    ~test_new();

private:
    Ui::test_new *ui;
    QLabel *item[80];   //测试项、数据显示部分
    QLabel *baseInfo[8];    //操作员、测试组等信息
    QLabel *barCode[2];     //条码
    QLabel *state;      //状态，合格、不合格、测试中
    QLabel *logo;
    QFrame *frame[8];
    QTextBrowser *itemInfo[8];
};

#endif // TEST_NEW_H
