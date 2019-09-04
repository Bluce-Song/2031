#ifndef PARAMETERBASE_H
#define PARAMETERBASE_H

#include <QWidget>
#include <QDebug>
#include <QFileDialog>
#include <QTime>
#include <QSettings>
#include <qt_windows.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "loginsetinfo.h"

extern testConfig tc;
namespace Ui {
class parameterBase;
}

class parameterBase : public QWidget
{
    Q_OBJECT

public:
    explicit parameterBase(QWidget *parent = 0);
    ~parameterBase();    

signals:
    void sendCommand(int index);
    void sendScanner(int cmd);

private:
    Ui::parameterBase *ui;

    HKEY hKey;
    LPCWSTR subkey;
    wchar_t keyname[256];
    char keyvalue[256];
    DWORD keysize,type,valuesize;
    int indexnum;

    void readParameter();
    void delay(int d);
    void inicomm();
    QString getcomm(int index,QString keyorvalue);

private slots:
    void saveParameter();
    void openClosePort();
    void openCloseSPort();
    void txtFilePath();
    void uploadSet();
    void setLength(QString t);
    void exitParameter();
};

#endif // PARAMETERBASE_H
