#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include "loginsetinfo.h"
#include "login.h"
#include <QWidget>
#include <QMainWindow>
#include <QDialog>
#include "test_main.h"

extern testConfig tc;

namespace Ui {
class loginDlg;
}

class loginDlg : public QDialog
{
    Q_OBJECT

public:
    explicit loginDlg(QDialog *parent = 0);
    ~loginDlg();

    int readIniFile();
    void writeIniFile();
    void checkLogin();

private slots:   
    void on_loginOK_clicked();
    void pwSetFocus();
    void readOperater();
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::loginDlg *ui;
    int reg;
    QStringList nameList;
    bool eventFilter(QObject *obj, QEvent *e);
};

#endif // LOGINDIALOG_H
