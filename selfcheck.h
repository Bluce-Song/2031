#ifndef SELFCHECK_H
#define SELFCHECK_H

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
class selfcheck;
}

class selfcheck : public QDialog
{
    Q_OBJECT

public:
    explicit selfcheck(QWidget *parent = 0);
    ~selfcheck();

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
    Ui::selfcheck *ui;
    QStandardItemModel *model;
    int type;
    int rowNum;

private slots:
    void openGroup();
    void openGroup(QModelIndex index);
    void newGroup();
    void sendGroupToMainThread();
    void exitGroup();
    void on_loginOK_clicked();
};

#endif // SELFCHECK_H
