#ifndef RANGE_H
#define RANGE_H

#include "loginsetinfo.h"
#include <QWidget>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>
#include <QComboBox>
#include <QTableWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QtSql/QSql>
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

namespace Ui {
class range;
}

class range : public QWidget
{
    Q_OBJECT

public:
    explicit range(QWidget *parent = 0);
    ~range();
    int changed;
    void readRangeFromDB(range_Item set[]);
    void tableInit();
private:
    Ui::range *ui;

public slots:
    void exitRange();
private slots:
    void saveRange();
    void onChanged(double value);
    void checkData(int row,int column);
    void checkData1(QTableWidgetItem *item);
};

#endif // RANGE_H
