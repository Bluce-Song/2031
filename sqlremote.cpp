#include "sqlremote.h"

sqlRemote::sqlRemote(QObject *parent) : QObject(parent)
{
    ip = tc.DS.ip;
    canDo = -1;
    sTimer = new QTimer(this);
    connect(sTimer, SIGNAL(timeout()), this, SLOT(readStatus()));
}

void sqlRemote::openDB(QString ip, int port, QString name, QString user, QString pw)
{
    openSQL(ip,port,name,user,pw);
}

void sqlRemote::setIpRemote()
{
    canDo = 0;
    ip = tc.DS.ip;
    canDo = 1;
}

void sqlRemote::openSQL(QString ip, int port, QString name, QString user, QString pw)
{
    if(db->isOpen())
        db->close();

    tc.sCanDO = 0;
    db->setDatabaseName(QString("DRIVER={SQL SERVER};SERVER=%1;PORT=%2;DATABASE=%3;UID=%4;PWD=%5;")
                         .arg(ip).arg(port).arg(name).arg(user).arg(pw));

    db->setPort(port);
    db->setUserName(user);
    db->setPassword(pw);
    db->setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=2;SQL_ATTR_CONNECTION_TIMEOUT=2");

    if(db->open())
    {
        tc.sCanDO = 1;
    }else{
        tc.sCanDO = -1;
    }

    qDebug()<<db->isOpen()<<db->isValid();
    db->close();
}

void sqlRemote::closeDB()
{
    db->close();
}

void sqlRemote::startTimer()
{
    sTimer->start(150);
}

void sqlRemote::stopTimer()
{
    sTimer->stop();
}

/***************************
 * 判断IP地址及端口是否在线
 * ************************/
bool sqlRemote::isAlive()
{
//    QTcpSocket tcpClient;
//    tcpClient.abort();
//    tcpClient.connectToHost(ip, port);
//    //100毫秒没有连接上则判断不在线
//    return tcpClient.waitForConnected(100);
}

void sqlRemote::readStatus()
{    
    int exitCode;

    QCoreApplication::processEvents();

    if(canDo == 0)
        return;

    QString cmd = "ping " + ip + " -n 1 -w 100";  //windows下的格式
    exitCode = QProcess::execute(cmd);
//    QProcess cmdPing;
//    cmdPing.start(cmd);
//    cmdPing.waitForReadyRead(100);
//    cmdPing.waitForFinished(100);

//    QString res = cmdPing.readAll();
//    qDebug() << res;
//    if (res.indexOf("TTL") == -1)
//    {

//    }
    QCoreApplication::processEvents();

    if(canDo == 0)
        return;

    if(0 == exitCode)
    {
        tc.sCanDO = 1;
    }else{
        tc.sCanDO = -1;
    }
}
