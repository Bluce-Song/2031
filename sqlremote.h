#ifndef SQLREMOTE_H
#define SQLREMOTE_H

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
class sqlRemote : public QObject
{
    Q_OBJECT
public:
    explicit sqlRemote(QObject *parent = 0);

signals:
    void sendRemoteOpen(int value);

public slots:
//    void write(QStringList list, QList<testDataStr> datalist);
    void setIpRemote();
    void startTimer();
    void stopTimer();
    void openDB(QString ip,int port,QString name,QString user,QString pw);
    void closeDB();

private slots:
    void readStatus(void);
    bool isAlive();

private:
    QSqlDatabase *db;

    int canDo;

    QTimer *sTimer;
    QString ip;
    void openSQL(QString ip, int port, QString name, QString user, QString pw);

};

#endif // SQLREMOTE_H
