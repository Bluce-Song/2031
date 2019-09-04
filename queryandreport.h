#ifndef QUERYANDREPORT_H
#define QUERYANDREPORT_H

#include <QWidget>
#include <QDebug>
#include <QDate>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QAxObject>
#include <QMessageBox>
#include <QScrollBar>
#include <QDesktopServices>
#include <QTableWidgetItem>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "loginsetinfo.h"

extern testConfig tc;
namespace Ui {
class queryAndReport;
}

class queryAndReport : public QWidget
{
    Q_OBJECT

public:
    explicit queryAndReport(QWidget *parent = 0);
    ~queryAndReport();

    void iniQuery(QString name);

private:
    Ui::queryAndReport *ui;
    QString seDate;
    QString userName;

private slots:
    void doQuery();
    void getReport();
    void makeReportQuickly();
    void on_exit_clicked();
    void readTestData(QTableWidgetItem* item);
};

#endif // QUERYANDREPORT_H
