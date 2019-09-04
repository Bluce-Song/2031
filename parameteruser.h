#ifndef PARAMETERUSER_H
#define PARAMETERUSER_H

#include <QWidget>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QtSql/QSql>
#include <QtSql/QSqlResult>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlIndex>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtSql/qsqldriver.h>
#include <QtSql/qsqlquery.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquerymodel.h>
#include "loginsetinfo.h"

extern testConfig tc;
namespace Ui {
class parameterUser;
}

class parameterUser : public QWidget
{
    Q_OBJECT

public:
    explicit parameterUser(QWidget *parent = 0);
    ~parameterUser();

private:
    Ui::parameterUser *ui;

    int newUser;
    void readParameter();

private slots:
    void readUserInfo(QTableWidgetItem* item);
    void addUser();
    void deleteUser();
    void saveUser();
    void setUserPassword();
    void confirmPassword();
    void savePassword();
    void exitUser();
};

#endif // PARAMETERUSER_H
