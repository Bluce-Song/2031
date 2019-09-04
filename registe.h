#ifndef REGISTE_H
#define REGISTE_H

#include <windows.h>
#include <QWidget>
#include <QDebug>
#include <qtsoap.h>
#include <QLibrary>
#include <QSettings>
#include <stdio.h>
#include <iostream>
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QStorageInfo>
#include <QDesktopServices>
#include "loginsetinfo.h"

extern testConfig tc;
namespace Ui {
class registe;
}

class registe : public QWidget
{
    Q_OBJECT

public:
    explicit registe(QWidget *parent = 0);
    ~registe();

private:
    Ui::registe *ui;
    void getCpuID();
    void GetCPUID2();
    QString getHDLogicalID();
    void registeInfo();
    void binaryFileTest();
    void webServiceTest();
    void GetLogicalDrives();
private slots:
    void exitReg();
    void onResponse(const QtSoapMessage &response);
};

#endif // REGISTE_H
