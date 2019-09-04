#ifndef PARAMETERRULE_H
#define PARAMETERRULE_H

#include <QWidget>
#include <QDebug>
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
class parameterRule;
}

class parameterRule : public QWidget
{
    Q_OBJECT

public:
    explicit parameterRule(QWidget *parent = 0);
    ~parameterRule();

private:
    Ui::parameterRule *ui;

    void readParameter();
    void getAllEncodingRules(int index);

private slots:
    void readEncodingRules(QString num);
    void readEncodingRules(QTableWidgetItem* item);
    void setMask(QTableWidgetItem* item);
    void setEncodingRulesLen(int len);
    void deleteEncodingRules();
    void saveEncodingRules();
    void exitRule();
};

#endif // PARAMETERRULE_H
