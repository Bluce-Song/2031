#ifndef PARAMETERRANGE_H
#define PARAMETERRANGE_H

#include <QWidget>
#include <QComboBox>
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
extern range_Item rangeSets[8];
extern QStringList itemUnits[8];
namespace Ui {
class parameterRange;
}

class parameterRange : public QWidget
{
    Q_OBJECT

public:
    explicit parameterRange(QWidget *parent = 0);
    ~parameterRange();

private:
    Ui::parameterRange *ui;

    int changed;
    void readParameter();
    void readRangeFromDB();
    void showRange();

private slots:
    void saveRange();
    void checkData(int row,int column);
    void checkItem(QTableWidgetItem *item);
    void exitRange();
};

#endif // PARAMETERRANGE_H
