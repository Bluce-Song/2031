#ifndef CHOOSEGROUPS_H
#define CHOOSEGROUPS_H

#include <QWidget>
#include <QDebug>
#include <QStandardItemModel>
#include <QTableView>
#include <QMessageBox>
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
class chooseGroups;
}

class chooseGroups : public QWidget
{
    Q_OBJECT

public:
    explicit chooseGroups(QWidget *parent = 0);
    ~chooseGroups();
    void readGroup();
    void setupModel();
    void setupView();

private:
    Ui::chooseGroups *ui;
    QStandardItemModel *model;
//    QTableView *table;    

private slots:
    void openGroup(QModelIndex index);
};

#endif // CHOOSEGROUPS_H
