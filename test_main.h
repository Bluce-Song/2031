#ifndef TEST_MAIN_H
#define TEST_MAIN_H
#include "stdio.h"
#include "loginsetinfo.h"
#include "range.h"
#include "serialport.h"
#include "sqloperate.h"
//#include "sqlremote.h"
#include "SerialWeiHuangE32.h"
#include "scanner.h"
#include "serialhfhaier.h"
#include "serialmultiple.h"
#include "serialportch.h"
#include "serialppl.h"
#include "seriallpc.h"
#include "serialxsb.h"
#include "dustfiltration.h"
#include "editGroups.h"
#include "ui_editgroups.h"
#include "choosegroup.h"
#include "ui_choosegroup.h"
#include "parameter.h"
#include "ui_parameter.h"
#include "parameterbase.h"
#include "ui_parameterbase.h"
#include "parameterfile.h"
#include "ui_parameterfile.h"
#include "parameteruser.h"
#include "ui_parameteruser.h"
#include "queryandreport.h"
#include "ui_queryandreport.h"
#include "parameterrange.h"
#include "ui_parameterrange.h"
#include "parametersenior.h"
#include "ui_parametersenior.h"
#include "parameterrule.h"
#include "ui_parameterrule.h"
#include "selfcheck.h"
#include "ui_selfcheck.h"
#include <windows.h>
#include <sys/time.h>
#include <QWidget>
#include <QThread>
#include <QMainWindow>
#include <QresizeEvent>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "loginsetinfo.h"
#include "ui_loginDialog.h"
#include "ui_test_main.h"
#include "ui_range.h"
#include "testitem.h"
#include "registe.h"
#include "testitemxsb.h"
#include <QObject>
#include <QMetaType>
#include <QSettings>
#include <QDialog>
#include <QPixmap>
#include <QTime>
#include <QTimer>
#include <QDir>
#include <QDate>
#include <QFile>
#include <QByteArray>
#include <QTextCodec>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <QLibrary>
#include <QChart>
#include <QtCharts>
#include <QChartView>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
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
#include <qtranslator>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTcpSocket>

QT_CHARTS_USE_NAMESPACE
extern int canTest;
extern testConfig tc;
extern range_Item rangeSets[8];
extern QStringList itemUnits[8];
extern QSerialPort *w_port;
extern QSerialPort *s_port;
extern int openType;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QMainWindow *parent = 0);
    ~MainWindow();

    int groupNum;
    int canDo;
    int sDo;

    groupConfig groupData;
//    void readIniFile();
    void testIni();
    int checkFileExists();    
    int readGroupFromFile(QString name);
    void readGroupTest();
    void showGroup();
    bool openPort(QSerialPort *sp,int port,int baudRate);
    char getChecksum(char *info, int start, int bytes);


signals:
    void findProgram(QString code);
    void writeSql(QStringList list,QList<testDataStr>);
    void sendCommand(quint16 addr, quint16 cmd, QByteArray data);
    void sendGroupData(groupConfig group);
    void sendStop();
    void sendScanner(int cmd);
    void sendAlarm(int index);
    void setIpRemoteFromparameter();
    void SerialPrint(QStringList list, QList<testDataStr> datalist);
private:
    Ui::MainWindow *ui;

    QNetworkReply *m_reply;
    int regState;
    int saveDb;
    int testStart;
    int testing;
    int PASS;
    int remind;
    int selfOffset;
    int psCanDo;
    int xlFlag;
    int iniFlag;
    float currentMax;
    float currentMax1;
    float psDataXSB;

    QTcpSocket *socket;
    QDate dateReg;
    QSerialPort *scannerPort;
    QChart *chart;
    QBarSet *set0;
    QBarSet *set1;
    QBarSeries *series;
    QBarCategoryAxis *axis;

    QThread *thread_WeiHuangE32;
    QThread *thread_sp;
    QThread *thread_sql;
//    QThread *thread_sqlRe;
    QThread *thread_sca;
    QThread *thread_df;
    serialPort serial;
    sqlOperate sqlOpe;
//    sqlRemote *sqlRe;
    SerialWeiHuangE32 WeiHuangE32Serial;
    serialHFHaier hfSerial;
    serialMultiple mSerial;
    serialportch chSerial;
    serialPPL pplSerial;
    serialLPC lpcSerial;
    serialXSB xsbSerial;
    dustFiltration *dustFiltr;
    testItemXSB *testItem_XSB;
    scanner scannerOpe;
    QList<testDataStr> testDataList;

    QList<testItem*> testItem_List;
    QList<QStandardItem*> standardItem_List;
    QStandardItemModel *model;

    QString barCodeScan;
    QString openGroupName;    
    QTimer *sysTimer;
    QTimer *tcpTimer;
    QTimer *alarmTimer;
    bool tcpFlag;
    QVBoxLayout *leftLayout;
    QGridLayout *rightLayout;
    QGridLayout *mainLayout;
    QLabel *info;

    groupConfig groupOffset;
    registeStr regStr;

    QModelIndex qindex;

    void initThreadAll(void);
    void initSerialWeiHuangE32(void);
    void initSerial(void);
    void initSql(void);
    void initScanner(void);
    void iniTcpScanner(void);
    void initParticleSensor(void);

    void delay(int d);
    void crateDataTable();
    void testDataItemSize(int step);
    void writeIniFile();
    void clearGroupTest(int flag);
    int findProgramByBarCode(QString barCode);
    void showSelfCheckGroup();
    void readSelfCheckGroup(QString name);
    void refreshBarChart();
    void makeTmpGroup();
    void iniBarChart();
    void barChartMemoryClear();
    void binaryFile();
    void openScanner();
    void closeScanner();
    int barCodeCheck(QString sn);
    int groupCheckPPL(QString name);
    void mdbFileFun();

private slots:
    int testStartFun();
    int stopTest();
    void getBarCode();
    void openCloseSerial(int index);
    void openCloseScanner(int cmd);
    void emptyPassFail();
    void systemTrigger();
    void reportTrigger();
    void registerTrigger();
    void rangeTrigger();
    void seniorTrigger();
    void roleTrigger();
    void groupTrigger();
    void configTrigger();
    void fileTrigger();
    void userTrigger();
    void chooseTrigger();
    void selfCheck();
    void readRangeFromDB(range_Item set[]);
    void receiveData(QString data);
    void sendGroupDataByEdit(int gn, QString fileName);
    void resizeEvent(QResizeEvent *);
//    void iniReadStatistics(QString fileName);
    void readMessage(int index, int cmd, testDataStr tds);
    void readBarCode(QString barCode);
    void readSensor1(float psData);
    void readSensor2(float psData);
    void readSensor3(float psData);
    void getCpuID();
    void readScannerData();
    void slotreply();
    void httpErrorShow();
    void sqlErrorShow(int index);
    void sDoSetValue(int v);
    void readTcpScanner();
    void tcpConnected();
    void tcpDisconnected();
    void onProgress();
    void onError(QAbstractSocket::SocketError);
    void setIpRemoteByPara();
    void alarmStyleSheet();
    void setItemErrorShow();
};

#endif // TEST_MAIN_H
