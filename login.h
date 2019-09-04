#ifndef LOGIN_H
#define LOGIN_H
#include "loginsetinfo.h"
#include <QWidget>
#include <QMainWindow>

namespace Ui {
class login;
}

class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();
    testConfig tc;
    void readIniFile();
    void writeIniFile();
    void checkLogin();

private slots:
    void on_loginOK_clicked();
private:
    Ui::login *ui;
};

#endif // LOGIN_H
