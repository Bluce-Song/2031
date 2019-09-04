#ifndef EDITGROUPS_H
#define EDITGROUPS_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QSettings>
#include <QTextCodec>
#include <QFile>
#include <QTime>
#include <QScrollBar>
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
#include <QCloseEvent>
#include "groupitem.h"
#include "choosegroup.h"
#include "loginsetinfo.h"
#include "test_main.h"

extern range_Item rangeSets[8];
extern int activeRow;
extern int openType;
extern testConfig tc;

namespace Ui {
class editGroups;
}

class editGroups : public QMainWindow
{
    Q_OBJECT

public:
    explicit editGroups(QWidget *parent = 0);
    ~editGroups();

    void readGroupFromFile(QString name);
    void setItmeSize();
    void newGroup();
    void singAndSlotConn();
    void setCommOffset(int value);

private:
    Ui::editGroups *ui;

    groupConfig groupData;
    QStringList itemUnits;
    QStringList getItemUnits(int index, int gnd);

    QList<groupItem*> groupItem_List;
    QList<QStandardItem*> standardItem_List;
    QStandardItemModel *model;

    int saveAs;
    int saved;
    int type;
    QString openGroupName;
    void showGroup();
    bool checkGroupName(QString name);
    void delay(int d);
    int checkIDCode(QString idCode);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void sendGroupFromEdit(int gn,QString fileName);

private slots:
    void on_add_clicked();
    void on_new_clicked();
    void on_open_clicked();
    void on_edit_clicked();
    void on_gnd_clicked(int index);
    void on_phase_clicked(int index);
    int on_save_clicked();
    void on_saveAs_clicked();
    void on_delete_clicked();
    void on_exit_clicked();
    void on_clicked_clicked(QModelIndex index);
    void receiveData(QString data);
    void receiveData(int index);
    void receiveChange(int index);
    void sendGroup();
};

#endif // EDITGROUPS_H
