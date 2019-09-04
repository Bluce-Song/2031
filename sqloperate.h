#ifndef SQLOPERATE_H
#define SQLOPERATE_H

#include <QFile>
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <qtsoap.h>
#include <QAxObject>
#include <QObject>
#include <QVariant>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDesktopServices>
#include <QCoreApplication>
#include "loginsetinfo.h"

extern testConfig tc;
extern range_Item rangeSets[8];
class sqlOperate : public QObject
{
    Q_OBJECT
public:
    explicit sqlOperate(QObject *parent = 0);

signals:
    void sendData();
    void httpError();
    void sqlError(int index);
    void sendsDo(int sd);
//    void openRemote(QString ip,int port,QString name,QString user,QString pw);

public slots:
    void write(QStringList list, QList<testDataStr> datalist);

//private slots:
//    int getRemoteOpen(int value);

private:
//    QSqlDatabase *dbRemoteSQL;

    QTimer *sTimer;
    QTimer *dTimer;
    QNetworkReply *m_reply;
    int count;
    QString code;
    QString barCode;
    void delay(int d);
    int resendData();
    int resendDataAIP();
    int resendDataIDI();
    int dataToHttp(QStringList list,QList<testDataStr> datalist);
    int testDataToLocalSQLAIP(QStringList list,QList<testDataStr> datalist);
    int testDataToLocalSQLIDI(QStringList list,QList<testDataStr> datalist);
    int testDataToLocalSQL(QStringList list,QList<testDataStr> datalist);
    int readPingStatus();

private slots:
    void slotreply();
};

#endif // SQLOPERATE_H
