#ifndef CHOOSEGROUP_H
#define CHOOSEGROUP_H

#include <QDialog>
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

extern int openType;
namespace Ui {
class chooseGroup;
}

class chooseGroup : public QDialog
{
    Q_OBJECT

public:
    explicit chooseGroup(QWidget *parent = 0);
    ~chooseGroup();
    void readGroup(QString name);
    void readSelfCheckGroup();
    void setupModel();
    void setupView();
    void setFillShow();

    int userLevel;

signals:
    QString sendData(QString);
    void sendGroupName(int,QString);

private:
    Ui::chooseGroup *ui;
    QStandardItemModel *model;
    int type;
    int rowNum;

private slots:
    void openGroup();
    void openGroup(QModelIndex index);
    void newGroup();
    void sendGroupToMainThread();
    void exitGroup();
};

#endif // CHOOSEGROUP_H
