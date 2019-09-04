#ifndef PARAMETER_H
#define PARAMETER_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QSettings>
#include <qt_windows.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "range.h"
#include "test_main.h"

extern int canTest;
extern testConfig tc;
extern range_Item rangeSets[8];
extern QStringList itemUnits[8];
extern QSerialPort *w_port;
extern QSerialPort *s_port;
namespace Ui {
class parameter;
}

class parameter : public QWidget
{
    Q_OBJECT

public:
    explicit parameter(QWidget *parent = 0);
    ~parameter();
    void readParameter(int index);
    void getAllEncodingRules(int index);

private:
    Ui::parameter *ui;
    QAction *actionA;
    int newUser;
    int changed;

    HKEY hKey;
    LPCWSTR subkey;
    wchar_t keyname[256];
    char keyvalue[256];
    DWORD keysize,type,valuesize;
    int indexnum;

    void readRangeFromDB();
    void showRange();
    void writeIniFile();
    void delay(int d);
    void inicomm();
    QString getcomm(int index,QString keyorvalue);
    void getSerialPortsByCS();

signals:
    void sendCommand(int index);
    void sendScanner(int cmd);
    void sendIpRemote();

private slots:
    void setStackedPage(int index);
    void checkAll();
    void deleteFile();
    void importFiles();
    void readEncodingRules(QString num);
    void readEncodingRules(QTableWidgetItem* item);
    void setMask(QTableWidgetItem* item);
    void setEncodingRulesLen(int len);
    void checkData(QTableWidgetItem* item);
    void checkData(int row,int column);
    void checkItem(QTableWidgetItem *item);
    void deleteEncodingRules();
    void saveEncodingRules();
    void readUserInfo(QTableWidgetItem* item);
    void addUser();
    void deleteUser();
    void saveUser();
    void setUserPassword();
    void confirmPassword();
    void savePassword();
    void saveRange();
    void saveSenior();
    void saveParameter();
    void exitParameter();
    void showPasswordItem();
    void hiddenPasswordItem();
    void setLength(QString t);
    void openClosePort();
    void openCloseSPort();
    void chooseFile(QModelIndex index);
    void txtFilePath();
    void uploadSet();
    void repeatSet(int index);
    void on_loginOK_clicked();
};

#endif // PARAMETER_H
