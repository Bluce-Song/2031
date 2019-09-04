#include "sqloperate.h"

sqlOperate::sqlOperate(QObject *parent) : QObject(parent)
{
    sTimer = new QTimer(this);
    sTimer->setInterval(4000);
    barCode = "";
}

int sqlOperate::resendData()
{
    QSqlDatabase *db,*db1,*db2;

    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","dbRE"));
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db->setDatabaseName(dsn);
    db->setUserName("");     //设置登陆数据库的用户名
    db->setPassword("");     //设置密码
    bool ok = db->open();

    if(!ok)
    {
        db->removeDatabase("dbRE");
        delete db;
        return -1;
    }
//    注意,对于express版本数据库, 一定要加\\sqlexpress这种后缀
//    QString dsn="DRIVER={SQL SERVER};SERVER=192.168.10.22\\sqlexpress;DATABASE=sqlscada";

    db1 = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC3","dbSQL"));
    db1->setDatabaseName(QString("DRIVER={SQL SERVER};SERVER=%1;PORT=%2;DATABASE=%3;UID=%4;PWD=%5;")
                         .arg(tc.DS.ip).arg(tc.DS.port).arg(tc.DS.dbName).arg(tc.DS.loginName).arg(tc.DS.password));

    db1->setPort(tc.DS.port);
    db1->setUserName(tc.DS.loginName);
    db1->setPassword(tc.DS.password);
    db1->setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=2;SQL_ATTR_CONNECTION_TIMEOUT=2");

    int sCanDO = -1;
    sCanDO = readPingStatus();

    if(sCanDO == -1)
    {
        emit sqlError(2);
        db->removeDatabase("dbRE");
        db->close();
        db1->removeDatabase("dbSQL");
        delete db1;
        delete db;
        return -2;
    }

    try
    {
        if(!db1->open())
        {
            qDebug()<<db1->lastError().text();
            emit sqlError(1);
            db->removeDatabase("dbRE");
            db->close();
            db1->removeDatabase("dbSQL");
            delete db1;
            delete db;
            return -2;
        }
    }catch(QString exception){
        qDebug()<<exception;
    }

//    emit sendsDo(0);
    QSqlQuery q(*db);
    QSqlQuery q1(*db1);

    QString sql = "select count(*) from resendList;";
    q.exec(sql);
    q.next();
    int num = q.value(0).toInt();
    q.clear();

    if(num == 0)
    {
        db1->removeDatabase("dbSQL");
        db1->close();
        db->removeDatabase("dbRE");
        db->close();
        delete db1;
        delete db;
        return 0;
    }

    sql = "select fileName from resendList;";
    q.exec(sql);
    while(q.next())
    {
        QString fl = q.value(0).toString();
        QString fn = QCoreApplication::applicationDirPath()+"/data/"+fl;

        if(!QFile::exists(fn))
        {
            QSqlQuery qq(*db);
            QString sqlDe = "delete * from resendList where fileName='"+fl+"';";
            bool ab = qq.exec(sqlDe);
            qq.clear();
            continue;
        }

        db2 = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","db2"));
        QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(fn);//连接字符串
        db2->setDatabaseName(dsn);    //设置连接字符串
        db2->setUserName("");     //设置登陆数据库的用户名
        db2->setPassword("");     //设置密码
        bool ok = db2->open();

        if(!ok)
        {
            continue;
        }
        QSqlQuery q2(*db2);

        //TestSummary表
        QString s = "select barCode,programName,operator,result,recordTime from TestSummary where num=-1;";
        q2.exec(s);
        while(q2.next())
        {
            QString rt = q2.value(4).toString();
            QString mes = QString("insert into TestSummary (barCode,programName,operator,result,recordTime,machineNum) VALUES('%1','%2','%3','%4','%5','%6');")
                    .arg(q2.value(0).toString()).arg(q2.value(1).toString()).arg(q2.value(2).toString())
                    .arg(q2.value(3).toString()).arg(rt).arg(tc.DS.machineNum);
            bool su = q1.exec(mes);
            q1.clear();

            if(su)
            {
                QSqlQuery q3(*db2);
                q3.prepare("update TestSummary set num=1 where recordTime=:v1");
                q3.bindValue(":v1",rt);
                q3.exec();
                q3.clear();
            }
        }
        q2.clear();

        //TestData表
        s = "select barCode,sequence,itemID,itemName,condition1,conditionUnit1,"
                      "condition2,conditionUnit2,condition3,conditionUnit3,condition4,conditionUnit4,"
                      "StaticDynamic,result,resultFlag,result1Data,result2Data,result3Data,result1Unit,"
                  "result2Unit,result3Unit,recordTime from TestData where num=-1;";

        q2.exec(s);
        while(q2.next())
        {
           int sq = q2.value(1).toInt();
           QString rt = q2.value(21).toString();

           QString mes = QString("insert into TestData (barCode,sequence,itemID,itemName,condition1,conditionUnit1,"
                                 "condition2,conditionUnit2,condition3,conditionUnit3,condition4,conditionUnit4,"
                                 "StaticDynamic,result,resultFlag,result1Data,result2Data,result3Data,result1Unit,"
                                 "result2Unit,result3Unit,recordTime,machineNum) VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9'"
                                 ",'%10','%11','%12','%13','%14','%15','%16','%17','%18','%19','%20','%21','%22','%23');")
                   .arg(q2.value(0).toString()).arg(q2.value(1).toInt()).arg(q2.value(2).toInt())
                   .arg(q2.value(3).toString()).arg(q2.value(4).toString())
                   .arg(q2.value(5).toString()).arg(q2.value(6).toString())
                   .arg(q2.value(7).toString()).arg(q2.value(8).toString())
                   .arg(q2.value(9).toString()).arg(q2.value(10).toString())
                   .arg(q2.value(11).toString()).arg(q2.value(12).toString())
                   .arg(q2.value(13).toString()).arg(q2.value(14).toString())
                   .arg(q2.value(15).toString()).arg(q2.value(16).toString())
                   .arg(q2.value(17).toString()).arg(q2.value(18).toString())
                   .arg(q2.value(19).toString()).arg(q2.value(20).toString())
                   .arg(q2.value(21).toString()).arg(tc.DS.machineNum);

           bool su = q1.exec(mes);
           q1.clear();

           if(su)
           {
               QSqlQuery q3(*db2);
               q3.prepare("update TestData set num=1 where sequence=:v1 and recordTime=:v2");
               q3.bindValue(":v1",sq);
               q3.bindValue(":v2",rt);
               q3.exec();
               q3.clear();
           }
        }
        db2->close();

        QSqlQuery q4(*db);
        QString cmd = "delete * from resendList where fileName='"+fl+"';";
        q4.exec(cmd);
        q4.clear();
    }
    q.clear();

    db1->removeDatabase("dbSQL");
    db->removeDatabase("dbRE");
    db1->close();
    db->close();
    delete db2;
    delete db1;
    delete db;
    return 0;
}

void sqlOperate::slotreply()
{
    QByteArray array = m_reply->readAll();
    QJsonParseError json_erron;
    QJsonDocument doc = QJsonDocument::fromJson(array,&json_erron);
    if(json_erron.error != QJsonParseError::NoError)
    {
        return;
    }

    if(doc.isObject())
    {
        QJsonObject obj = doc.object();

        if(obj.contains("success"))
        {
            QJsonValue value = obj.value("success");
            if(value.isBool())
            {
                bool v = value.toBool();
                qDebug()<<v;
            }
        }
    }
}

int sqlOperate::dataToHttp(QStringList list,QList<testDataStr> datalist)
{  
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest *request = new QNetworkRequest();
    QNetworkRequest *request1 = new QNetworkRequest();

    request->setUrl(QUrl(tc.url_2));
    request->setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    request1->setUrl(QUrl(tc.url_3));
    request1->setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QByteArray postArray;

    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString value = QString("productCode=%1&programName=%2&operatorUser=%3&detectResult=%4&detectDate=%5")
            .arg(list.at(0)).arg(list.at(1)).arg(list.at(2)).arg(list.at(3)).arg(saveTime);

    postArray.append(value);
    m_reply = manager->post(*request,postArray);

    QEventLoop loop;
    connect(sTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    sTimer->start();
    loop.exec();

    if(sTimer->isActive())
    {
        sTimer->stop();
        if(m_reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "Error String : " << m_reply->errorString();
            emit httpError();
            loop.deleteLater();
            m_reply = NULL;
            manager->deleteLater();
            delete request;
            delete request1;            
            return -1;
        }else{
            QByteArray array = m_reply->readAll();
            QJsonParseError json_erron;
            QJsonDocument doc = QJsonDocument::fromJson(array,&json_erron);
            if(json_erron.error != QJsonParseError::NoError)
            {
                qDebug()<<json_erron.errorString();
                loop.deleteLater();
                m_reply = NULL;
                manager->deleteLater();
                delete request;
                delete request1;
                emit httpError();
                return -1;
            }

            if(doc.isObject())
            {
                QJsonObject obj = doc.object();

                if(obj.contains("success"))
                {
                    QJsonValue jValue = obj.value("success");
                    if(jValue.isBool())
                    {
                        bool v = jValue.toBool();
                        if(v == false)
                        {
                            loop.deleteLater();
                            m_reply = NULL;
                            manager->deleteLater();
                            delete request;
                            delete request1;
                            emit httpError();
                            return -1;
                        }

                        for(int i=0;i<datalist.size();i++)
                        {
                            QString name = NULL;
                            int result = 0;
                            QStringList data;
                            switch(datalist.at(i).model)
                            {
                            case 0:
                                name = tr("无效");
                                data<<"";
                                data<<"";
                                data<<"";
                                data<<"";
                                data<<"";
                                data<<"";
                                break;
                            case 1:
                                name = tr("交耐");
                                data<<QString::number(datalist.at(i).voltage,10,rangeSets[0].prec[0]);
                                data<<QString::number(datalist.at(i).current,10,rangeSets[0].prec[1]);
                                data<<"";
                                data<<rangeSets[0].unit[0];
                                data<<rangeSets[0].unit[1];
                                data<<"";
                                break;
                            case 2:
                                name = tr("直耐");
                                data<<QString::number(datalist.at(i).voltage,10,rangeSets[1].prec[0]);
                                data<<QString::number(datalist.at(i).current,10,rangeSets[1].prec[1]);
                                data<<"";
                                data<<rangeSets[1].unit[0];
                                data<<rangeSets[1].unit[1];
                                data<<"";
                                break;
                            case 3:
                                name = tr("绝缘");
                                data<<QString::number(datalist.at(i).voltage,10,rangeSets[2].prec[0]);
                                data<<QString::number(datalist.at(i).resistance,10,rangeSets[2].prec[1]);
                                data<<"";
                                data<<rangeSets[2].unit[0];
                                data<<rangeSets[2].unit[1];
                                data<<"";
                                break;
                            case 4:
                                name = tr("接地");
                                data<<QString::number(datalist.at(i).current,10,rangeSets[3].prec[0]);
                                data<<QString::number(datalist.at(i).resistance,10,rangeSets[3].prec[1]);
                                data<<"";
                                data<<rangeSets[3].unit[3];
                                data<<rangeSets[3].unit[1];
                                data<<"";
                                break;
                            case 5:
                                name = tr("泄漏");
                                data<<QString::number(datalist.at(i).voltage,10,rangeSets[4].prec[0]);
                                data<<QString::number(datalist.at(i).current,10,rangeSets[4].prec[1]);
                                data<<"";
                                data<<rangeSets[4].unit[0];
                                data<<rangeSets[4].unit[1];
                                data<<"";
                                break;
                            case 6:
                                name = tr("功率");
                                data<<QString::number(datalist.at(i).voltage,10,rangeSets[5].prec[0]);
                                data<<QString::number(datalist.at(i).current,10,rangeSets[5].prec[3]);
                                data<<QString::number(datalist.at(i).power,10,rangeSets[5].prec[1]);
                                data<<rangeSets[5].unit[0];
                                data<<rangeSets[5].unit[3];
                                data<<rangeSets[5].unit[1];
                                break;
                            case 7:
                                name = tr("低启");
                                data<<QString::number(datalist.at(i).voltage,10,rangeSets[6].prec[0]);
                                data<<QString::number(datalist.at(i).current,10,rangeSets[6].prec[1]);
                                data<<QString::number(datalist.at(i).power,10,rangeSets[6].prec[3]);
                                data<<rangeSets[6].unit[0];
                                data<<rangeSets[6].unit[1];
                                data<<rangeSets[6].unit[3];
                                break;
                            case 8:
                                name = tr("开短");
                                data<<QString::number(datalist.at(i).voltage,10,rangeSets[7].prec[0]);
                                data<<QString::number(datalist.at(i).resistance,10,rangeSets[7].prec[1]);
                                data<<"";
                                data<<rangeSets[7].unit[0];
                                data<<rangeSets[7].unit[1];
                                data<<"";
                                break;
                            }

                            if(datalist.at(i).flag1 == 1)
                            {
                                result = 1;
                            }else{
                                result = 0;
                            }

                            QString resultFlag = QString("%1,%2,%3,%4,%5,%6").arg(datalist.at(i).flag3).arg(datalist.at(i).flag4)
                                    .arg(datalist.at(i).flag5).arg(datalist.at(i).flag6).arg(datalist.at(i).flag7).arg(datalist.at(i).flag8);

                            QString cd = list.at(4+datalist.at(i).step);
                            QStringList cdList = cd.split(",");

                            QString sn;
                            if(list.at(0) == NULL)
                                sn = "";
                            else
                                sn = list.at(0);

                            QString value1 = QString("productCode=%1&detectSequence=%2&detectItemId=%3&detectItemName=%4&condition1=%5"
                                                     "&conditionUnit1=%6&condition2=%7&conditionUnit2=%8&condition3=%9&conditionUnit3=%10"
                                                     "&condition4=%11&conditionUnit4=%12&staticDynamic=%13&detectResult=%14"
                                                     "&detectResultFlag=%15&result1Data=%16&result2Data=%17&result3Data=%18"
                                                     "&result1Unit=%19&result2Unit=%20&result3Unit=%21&detectDate=%22")
                                    .arg(list.at(0)).arg(i+1).arg(datalist.at(i).model).arg(name).arg(cdList.at(0)).arg(cdList.at(1))
                                    .arg(cdList.at(2)).arg(cdList.at(3)).arg(cdList.at(4)).arg(cdList.at(5)).arg(cdList.at(6)).arg(cdList.at(7))
                                    .arg(cdList.at(8)).arg(result).arg(resultFlag).arg(data.at(0)).arg(data.at(1)).arg(data.at(2)).arg(data.at(3))
                                    .arg(data.at(4)).arg(data.at(5)).arg(saveTime);

                            postArray.clear();
                            m_reply = NULL;

                            postArray.append(value1);
                            m_reply = manager->post(*request1,postArray);
                            qDebug()<<value1;
                            delay(100);
                        }

                        loop.deleteLater();
                        m_reply->deleteLater();
                        m_reply = NULL;
                        manager->deleteLater();
                        delete request;
                        delete request1;
                    }
                }
            }
        }
    }else{
        emit httpError();
        disconnect(m_reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        m_reply->abort();
        m_reply->deleteLater();
        return -1;
    }
    return 0;
}

void sqlOperate::delay(int d)
{
    QTime dieTime = QTime::currentTime().addMSecs(d);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents();
    }
}

int sqlOperate::resendDataIDI()
{
    QSqlDatabase *db,*db1,*db2;

    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","dbRE"));
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db->setDatabaseName(dsn);
    db->setUserName("");     //设置登陆数据库的用户名
    db->setPassword("");     //设置密码
    bool ok = db->open();

    if(!ok)
    {
        db->removeDatabase("dbRE");
        delete db;
        return -1;
    }
//    注意,对于express版本数据库, 一定要加\\sqlexpress这种后缀
//    QString dsn="DRIVER={SQL SERVER};SERVER=192.168.10.22\\sqlexpress;DATABASE=sqlscada";

    db1 = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC3","dbSQL"));
    db1->setDatabaseName(QString("DRIVER={SQL SERVER};SERVER=%1;PORT=%2;DATABASE=%3;UID=%4;PWD=%5;")
                         .arg(tc.DS.ip).arg(tc.DS.port).arg(tc.DS.dbName).arg(tc.DS.loginName).arg(tc.DS.password));

    db1->setPort(tc.DS.port);
    db1->setUserName(tc.DS.loginName);
    db1->setPassword(tc.DS.password);
    db1->setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=2;SQL_ATTR_CONNECTION_TIMEOUT=2");

    int sCanDO = -1;
    sCanDO = readPingStatus();

    if(sCanDO == -1)
    {
        emit sqlError(2);
        db->removeDatabase("dbRE");
        db->close();
        db1->removeDatabase("dbSQL");
        delete db1;
        delete db;
        return -2;
    }

    try
    {
        if(!db1->open())
        {
            qDebug()<<db1->lastError().text();
            emit sqlError(1);
            db->removeDatabase("dbRE");
            db->close();
            db1->removeDatabase("dbSQL");
            delete db1;
            delete db;
            return -2;
        }
    }catch(QString exception){
        qDebug()<<exception;
    }

//    emit sendsDo(0);
    QSqlQuery q(*db);
    QSqlQuery q1(*db1);

    QString sql = "select count(*) from resendList;";
    q.exec(sql);
    q.next();
    int num = q.value(0).toInt();
    q.clear();

    if(num == 0)
    {
        db1->removeDatabase("dbSQL");
        db1->close();
        db->removeDatabase("dbRE");
        db->close();
        delete db1;
        delete db;
        return 0;
    }

    sql = "select fileName from resendList;";
    q.exec(sql);
    while(q.next())
    {
        QString fl = q.value(0).toString();
        QString fn = QCoreApplication::applicationDirPath()+"/data/"+fl;

        if(!QFile::exists(fn))
        {
            QSqlQuery qq(*db);
            QString sqlDe = "delete * from resendList where fileName='"+fl+"';";
            bool ab = qq.exec(sqlDe);
            qq.clear();
            continue;
        }

        db2 = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","db2"));
        QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(fn);//连接字符串
        db2->setDatabaseName(dsn);    //设置连接字符串
        db2->setUserName("");     //设置登陆数据库的用户名
        db2->setPassword("");     //设置密码
        bool ok = db2->open();

        if(!ok)
        {
            continue;
        }
        QSqlQuery q2(*db2);

        //TestSummary表
        QString s = "select barCode,programName,operator,result,recordTime from TestSummary where num=-1;";
        q2.exec(s);
        while(q2.next())
        {
            QString rt = q2.value(4).toString();
//            QString trFlag = "";
//            if(q2.value(3).toString() == "1")
//            {
//                trFlag = "PASS";
//            }else{
//                trFlag = "FAIL";
//            }

            QString mes = QString("insert into TestInfoBaseSerialNo (SerialNo,TestResult,SystemTime,DeviceID) VALUES('%1','%2','%3','%4');")
                    .arg(q2.value(0).toString()).arg(q2.value(3).toString()).arg(rt).arg(tc.DS.machineNum);
            bool su = q1.exec(mes);
            q1.clear();

            if(su)
            {
                QSqlQuery q3(*db2);
                q3.prepare("update TestSummary set num=1 where recordTime=:v1");
                q3.bindValue(":v1",rt);
                q3.exec();
                q3.clear();
            }
        }
        q2.clear();

        //TestData表
        s = "select barCode,sequence,itemID,itemName,condition1,conditionUnit1,"
                      "condition2,conditionUnit2,condition3,conditionUnit3,condition4,conditionUnit4,"
                      "StaticDynamic,result,resultFlag,result1Data,result2Data,result3Data,result1Unit,"
                  "result2Unit,result3Unit,recordTime from TestData where num=-1;";

        q2.exec(s);
        while(q2.next())
        {
            int sq = q2.value(1).toInt();
            QString rt = q2.value(21).toString();

            int tiid = 0;
            QString data,unit,conSet,conOut,conUnit;
            switch(q2.value(2).toInt())
            {
            case 0:
                conSet = "";
                conOut = "";
                conUnit = "";
                data = "";
                unit = "";
                tiid = 0;
                break;
            case 1:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(16).toString();
                unit = q2.value(19).toString();
                tiid = 1;
                break;
            case 2:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(16).toString();
                unit = q2.value(19).toString();
                tiid = 1;
                break;
            case 3:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(16).toString();
                unit = q2.value(19).toString();
                tiid = 3;
                break;
            case 4:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(16).toString();
                unit = q2.value(19).toString();
                tiid = 2;
                break;
            case 5:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(16).toString();
                unit = q2.value(19).toString();
                tiid = 4;
                break;
            case 6:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(17).toString();
                unit = q2.value(20).toString();
                tiid = 5;
                break;
            case 7:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(17).toString();
                unit = q2.value(20).toString();
                tiid = 6;
                break;
            case 8:
                conSet = q2.value(4).toString();
                conOut = q2.value(15).toString();
                conUnit = q2.value(5).toString();
                data = q2.value(16).toString();
                unit = q2.value(19).toString();
                tiid = 7;
                break;
            }

            QString mes = QString("insert into TestConditionResult (SerialNo,TIID,TIName,ConditionSet,ConditionOut,ConditionUnit,ResultValue,ResultUnit,Result,SystemTime) "
                                  "VALUES('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10');")
                    .arg(q2.value(0).toString()).arg(tiid).arg(q2.value(3).toString())
                    .arg(conSet).arg(conOut).arg(conUnit).arg(data).arg(unit)
                    .arg(q2.value(13).toString()).arg(q2.value(21).toString());

            bool su = q1.exec(mes);
            q1.clear();

            if(su)
            {
                QSqlQuery q3(*db2);
                q3.prepare("update TestData set num=1 where sequence=:v1 and recordTime=:v2");
                q3.bindValue(":v1",sq);
                q3.bindValue(":v2",rt);
                q3.exec();
                q3.clear();
            }
        }
        db2->close();

        QSqlQuery q4(*db);
        QString cmd = "delete * from resendList where fileName='"+fl+"';";
        q4.exec(cmd);
        q4.clear();
    }
    q.clear();

    db1->removeDatabase("dbSQL");
    db->removeDatabase("dbRE");
    db1->close();
    db->close();
    delete db2;
    delete db1;
    delete db;
    return 0;
}

int sqlOperate::resendDataAIP()
{
    QSqlDatabase *db,*db1,*db2;

    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","dbRE"));
    QString sDbNm = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db->setDatabaseName(dsn);
    db->setUserName("");     //设置登陆数据库的用户名
    db->setPassword("");     //设置密码
    bool ok = db->open();

    if(!ok)
    {
        db->removeDatabase("dbRE");
        delete db;
        return -1;
    }

    db1 = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC3","dbSQL"));
    db1->setDatabaseName(QString("DRIVER={SQL SERVER};SERVER=%1;PORT=%2;DATABASE=%3;UID=%4;PWD=%5;")
                         .arg(tc.DS.ip).arg(tc.DS.port).arg(tc.DS.dbName).arg(tc.DS.loginName).arg(tc.DS.password));

    db1->setPort(tc.DS.port);
    db1->setUserName(tc.DS.loginName);
    db1->setPassword(tc.DS.password);
    db1->setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=2;SQL_ATTR_CONNECTION_TIMEOUT=2");

    int sCanDO = -1;
    sCanDO = readPingStatus();

    if(sCanDO == -1)
    {
        emit sqlError(2);
        db->removeDatabase("dbRE");
        db->close();
        db1->removeDatabase("dbSQL");
        delete db1;
        delete db;
        return -2;
    }

    try
    {
        if(!db1->open())
        {
            emit sqlError(1);
            db->removeDatabase("dbRE");
            db->close();
            db1->removeDatabase("dbSQL");
            delete db1;
            delete db;
            return -2;
        }
    }catch(QString exception){
        qDebug()<<exception;
    }

    QSqlQuery q(*db);
    QSqlQuery q1(*db1);

    QString sql = "select count(*) from resendList;";
    q.exec(sql);
    q.next();
    int num = q.value(0).toInt();
    q.clear();

    if(num == 0)
    {
        db1->removeDatabase("dbSQL");
        db1->close();
        db->removeDatabase("dbRE");
        db->close();
        delete db1;
        delete db;
        return 0;
    }

    sql = "select fileName from resendList;";
    q.exec(sql);
    while(q.next())
    {
        QString fl = q.value(0).toString();
        QString fn = QCoreApplication::applicationDirPath()+"/data/"+fl;

        if(!QFile::exists(fn))
        {
            QSqlQuery qq(*db);
            QString sqlDe = "delete * from resendList where fileName='"+fl+"';";
            bool ab = qq.exec(sqlDe);
            qq.clear();
            continue;
        }

        db2 = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","db2"));
        QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(fn);//连接字符串
        db2->setDatabaseName(dsn);    //设置连接字符串
        db2->setUserName("");     //设置登陆数据库的用户名
        db2->setPassword("");     //设置密码
        bool ok = db2->open();

        if(!ok)
        {
            continue;
        }
        QSqlQuery q2(*db2);
        QSqlQuery q5(*db2);

        //TestSummary表
        QString s = "select barCode,programName,operator,result,recordTime from TestSummary where num=-1;";
        q2.exec(s);
        while(q2.next())
        {            
            QString sn = q2.value(0).toString();
            QString pn = q2.value(1).toString();
            QString op = q2.value(2).toString();
            QString re = q2.value(3).toString();
            QString rt = q2.value(4).toString();

            q1.prepare("insert into UDT_QM_SafetyCheck (BarCode,TestTime,TestGroup,Operator,Result,"
                      "TestItem1,Item1Tag1,Item1Data1,Item1Tag2,Item1Data2,Item1Tag3,Item1Data3,Item1Mode,Item1Result,"
                      "TestTiem2,Item2Tag1,Item2Data1,Item2Tag2,Item2Data2,Item2Tag3,Item2Data3,Item2Mode,Item2Result,"
                      "TestTiem3,Item3Tag1,Item3Data1,Item3Tag2,Item3Data2,Item3Tag3,Item3Data3,Item3Mode,Item3Result,"
                      "TestTiem4,Item4Tag1,Item4Data1,Item4Tag2,Item4Data2,Item4Tag3,Item4Data3,Item4Mode,Item4Result,MachineNum)"
                      " VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13,:v14,:v15,:v16,:v17,:v18,:v19,:v20"
                      ",:v21,:v22,:v23,:v24,:v25,:v26,:v27,:v28,:v29,:v30,:v31,:v32,:v33,:v34,:v35,:v36,:v37,:v38,:v39,:v40"
                      ",:v41,:v42);");

            q1.bindValue(":v1",sn);
            q1.bindValue(":v2",rt);
            q1.bindValue(":v3",pn);
            q1.bindValue(":v4",op);

            s = "select barCode,sequence,itemID,itemName,StaticDynamic,result,result1Data,result2Data,result3Data,"
                "result1Unit,result2Unit,result3Unit from TestData where recordTime='" + rt +"' and num=-1;";
            q5.exec(s);
            int jn = 0;
            int jy = 0;
            int jd = 0;
            int xl = 0;
            while(q5.next())
            {
                int sq = q5.value(1).toInt();
                int id = q5.value(2).toInt();

                if(re == "0")
                    q1.bindValue(":v5",tr("不合格"));
                else
                    q1.bindValue(":v5",tr("合格"));

                switch(id)
                {
                    case 1:
                        jn++;
                        q1.bindValue(":v6",tr("交耐"));
                        q1.bindValue(":v7",q5.value(9).toString());
                        q1.bindValue(":v8",q5.value(6).toString());
                        q1.bindValue(":v9",q5.value(10).toString());
                        q1.bindValue(":v10",q5.value(7).toString());
                        q1.bindValue(":v11","");
                        q1.bindValue(":v12","0.0");
                        q1.bindValue(":v13","");
                        if(q5.value(5).toInt() == 1)
                        {
                            q1.bindValue(":v14",tr("合格"));
                        }else{
                            q1.bindValue(":v14",tr("不合格"));
                        }
                        break;
                    case 3:
                        jy++;
                        q1.bindValue(":v15",tr("绝缘"));
                        q1.bindValue(":v16",q5.value(9).toString());
                        q1.bindValue(":v17",q5.value(6).toString());
                        q1.bindValue(":v18",q5.value(10).toString());
                        q1.bindValue(":v19",q5.value(7).toString());
                        q1.bindValue(":v20","");
                        q1.bindValue(":v21","0.0");
                        q1.bindValue(":v22","");
                        if(q5.value(5).toInt() == 1)
                        {
                            q1.bindValue(":v23",tr("合格"));
                        }else{
                            q1.bindValue(":v23",tr("不合格"));
                        }
                        break;
                    case 4:
                        jd++;
                        q1.bindValue(":v24",tr("接地"));
                        q1.bindValue(":v25",q5.value(9).toString());
                        q1.bindValue(":v26",q5.value(6).toString());
                        q1.bindValue(":v27",q5.value(10).toString());
                        q1.bindValue(":v28",q5.value(7).toString());
                        q1.bindValue(":v29","");
                        q1.bindValue(":v30","0.0");
                        q1.bindValue(":v31","");
                        if(q5.value(5).toInt() == 1)
                        {
                            q1.bindValue(":v32",tr("合格"));
                        }else{
                            q1.bindValue(":v32",tr("不合格"));
                        }
                        break;
                    case 5:
                        xl++;
                        q1.bindValue(":v33",tr("泄漏"));
                        q1.bindValue(":v34",q5.value(9).toString());
                        q1.bindValue(":v35",q5.value(6).toString());
                        q1.bindValue(":v36",q5.value(10).toString());
                        q1.bindValue(":v37",q5.value(7).toString());
                        q1.bindValue(":v38","");
                        q1.bindValue(":v39","0.0");

                        QString cd = q5.value(4).toString();
                        QStringList cdList = cd.split("|");

                        if(!cdList.isEmpty())
                        {
                            if(cdList.at(1) == "0")
                                q1.bindValue(":v40","静态");
                            else
                                q1.bindValue(":v40","动态");
                        }else{
                            q1.bindValue(":v40","");
                        }

                        if(q5.value(5).toInt() == 1)
                        {
                            q1.bindValue(":v41",tr("合格"));
                        }else{
                            q1.bindValue(":v41",tr("不合格"));
                        }
                        break;
                }
            }

            if(jn == 0)
            {
                q1.bindValue(":v6",tr("无(未测试)"));
                q1.bindValue(":v7","");
                q1.bindValue(":v8","0.0");
                q1.bindValue(":v9","");
                q1.bindValue(":v10","0.0");
                q1.bindValue(":v11","");
                q1.bindValue(":v12","0.0");
                q1.bindValue(":v13","——");
                q1.bindValue(":v14","——");
            }

            if(jy == 0)
            {
                q1.bindValue(":v15",tr("无(未测试)"));
                q1.bindValue(":v16","");
                q1.bindValue(":v17","0.0");
                q1.bindValue(":v18","");
                q1.bindValue(":v19","0.0");
                q1.bindValue(":v20","");
                q1.bindValue(":v21","0.0");
                q1.bindValue(":v22","——");
                q1.bindValue(":v23","——");
            }

            if(jd == 0)
            {
                q1.bindValue(":v24",tr("无(未测试)"));
                q1.bindValue(":v25","");
                q1.bindValue(":v26","0.0");
                q1.bindValue(":v27","");
                q1.bindValue(":v28","0.0");
                q1.bindValue(":v29","");
                q1.bindValue(":v30","0.0");
                q1.bindValue(":v31","——");
                q1.bindValue(":v32","——");
            }

            if(xl == 0)
            {
                q1.bindValue(":v33",tr("无(未测试)"));
                q1.bindValue(":v34","");
                q1.bindValue(":v35","0.0");
                q1.bindValue(":v36","");
                q1.bindValue(":v37","0.0");
                q1.bindValue(":v38","");
                q1.bindValue(":v39","0.0");
                q1.bindValue(":v40","——");
                q1.bindValue(":v41","——");
            }
            q1.bindValue(":v42",tc.DS.machineNum);

            bool su = q1.exec();
            q1.clear();

            if(su)
            {
                QSqlQuery q3(*db2);
                q3.prepare("update TestSummary set num=1 where recordTime=:v1;");
                q3.bindValue(":v1",rt);
                q3.exec();
                q3.clear();
                q3.prepare("update TestData set num=1 where recordTime=:v1 and num=-1;");
                q3.bindValue(":v1",rt);
                q3.exec();
                q3.clear();
            }
        }
        q2.clear();        
        db2->close();

        QSqlQuery q4(*db);
        QString cmd = "delete * from resendList where fileName='"+fl+"';";
        q4.exec(cmd);
        q4.clear();
    }
    q.clear();

    db1->removeDatabase("dbSQL");
    db->removeDatabase("dbRE");
    db1->close();
    db->close();
    delete db2;
    delete db1;
    delete db;
    return 0;
}

int sqlOperate::testDataToLocalSQLIDI(QStringList list,QList<testDataStr> datalist)
{
    int resend = resendDataIDI();
    if(resend == -2)
    {
        return -1;
    }    

    QSqlDatabase *db;
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","db"));
    db->setDatabaseName(QString("DRIVER={SQL SERVER};SERVER=%1;DATABASE=%2;UID=%3;PWD=%4;")
                       .arg(tc.DS.ip).arg(tc.DS.dbName).arg(tc.DS.loginName).arg(tc.DS.password));
    db->setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=2;SQL_ATTR_CONNECTION_TIMEOUT=2");

    int sCanDO = -1;
    sCanDO = readPingStatus();

    if(sCanDO == -1)
    {
        emit sqlError(2);
        delete db;
        return -1;
    }

    if(!db->open())
    {
        emit sqlError(1);
        delete db;
        return -1;
    }

//    QString trFlag = "";
//    if(list.at(3) == "1")
//    {
//        trFlag = "PASS";
//    }else{
//        trFlag = "FAIL";
//    }

    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //写TestInfoBaseSerialNo表
    QSqlQuery q(*db);
    QString sql = "";

    if(tc.STD.repeat == 1)
    {
        if(tc.STD.repeatType == 1)
        {
            sql = QString("delete from TestInfoBaseSerialNo where SerialNo='%1';")
                    .arg(list.at(0));
            q.exec(sql);
            q.clear();

            sql = QString("delete from TestConditionResult where SerialNo='%1';")
                    .arg(list.at(0));
            q.exec(sql);
            q.clear();
        }else{
            sql = QString("select count(*) from TestInfoBaseSerialNo where SerialNo='%1';")
                    .arg(list.at(0));
            q.exec(sql);
            q.next();
            int num = q.value(0).toInt();
            q.clear();

            if(num > 0)
            {
                db->close();
                delete db;
                return 0;
            }
        }
    }

    sql = QString("insert into TestInfoBaseSerialNo (SerialNo,TestResult,SystemTime,DeviceID) VALUES('%1','%2','%3','%4');")
            .arg(list.at(0)).arg(list.at(3)).arg(saveTime).arg(tc.DS.machineNum);
    bool ok = q.exec(sql);

    //写TestConditionResult表
    int tiid = 0;
    for(int i=0;i<datalist.size();i++)
    {
        q.clear();
        QString name = "";
        int result = 0;
        QString data,unit,conSet,conOut,conUnit;

        QString cd = list.at(4+datalist.at(i).step);
        QStringList cdList = cd.split(",");

        switch(datalist.at(i).model)
        {
            case 0:
            {
                name = tr("无效");
                conSet = "";
                conOut = "";
                conUnit = "";
                data = "";
                unit = "";
                tiid = 0;
            }
                break;
            case 1:
            {
                name = tr("交耐");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).voltage,10,rangeSets[0].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).current,10,rangeSets[0].prec[1]);
                unit = rangeSets[0].unit[1];
                tiid = 1;
            }
                break;
            case 2:
            {
                name = tr("直耐");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).voltage,10,rangeSets[1].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).current,10,rangeSets[1].prec[1]);
                unit = rangeSets[1].unit[1];
                tiid = 1;
            }
                break;
            case 3:
            {
                name = tr("绝缘");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).voltage,10,rangeSets[2].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).resistance,10,rangeSets[2].prec[1]);
                unit = rangeSets[2].unit[1];
                tiid = 3;
            }
                break;
            case 4:
            {
                name = tr("接地");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).current,10,rangeSets[3].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).resistance,10,rangeSets[3].prec[1]);
                unit = rangeSets[3].unit[1];
                tiid = 2;
            }
                break;
            case 5:
            {
                name = tr("泄漏");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).voltage,10,rangeSets[4].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).current,10,rangeSets[4].prec[1]);
                unit = rangeSets[4].unit[1];
                tiid = 4;
            }
                break;
            case 6:
            {
                name = tr("功率");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).voltage,10,rangeSets[5].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).power,10,rangeSets[5].prec[1]);
                unit = rangeSets[5].unit[1];
                tiid = 5;
            }
                break;
            case 7:
            {
                name = tr("低启");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).voltage,10,rangeSets[6].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).power,10,rangeSets[6].prec[3]);
                unit = rangeSets[6].unit[3];
                tiid = 6;
            }
                break;
            case 8:
            {
                name = tr("开短");
                conSet = cdList.at(0);
                conOut = QString::number(datalist.at(i).voltage,10,rangeSets[7].prec[0]);
                conUnit = cdList.at(1);
                data = QString::number(datalist.at(i).resistance,10,rangeSets[7].prec[1]);
                unit = rangeSets[7].unit[1];
                tiid = 7;
            }
                break;
        }

        if(datalist.at(i).flag1 == 1)
        {
            result = 1;
        }else{
            result = 0;
        }

        QString sn;
        if(list.at(0) == NULL)
            sn = "";
        else
            sn = list.at(0);

        q.prepare("insert into TestConditionResult (SerialNo,TIID,TIName,ConditionSet,ConditionOut,ConditionUnit,ResultMin,ResultMax,ResultValue,ResultUnit,Result,SystemTime) "
                  "VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12);");

        q.bindValue(":v1",sn);
        q.bindValue(":v2",tiid);
        q.bindValue(":v3",name);
        q.bindValue(":v4",conSet);
        q.bindValue(":v5",conOut);
        q.bindValue(":v6",conUnit);
        q.bindValue(":v7",cdList.at(2));
        q.bindValue(":v8",cdList.at(4));
        q.bindValue(":v9",data);
        q.bindValue(":v10",unit);
        q.bindValue(":v11",result);
        q.bindValue(":v12",saveTime);

        ok = q.exec();
    }

    db->close();
    delete db;
    return 0;
}

int sqlOperate::testDataToLocalSQLAIP(QStringList list,QList<testDataStr> datalist)
{
    int resend = resendDataAIP();
    if(resend == -2)
    {
        return -1;
    }

    QSqlDatabase *db;
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","db"));
    db->setDatabaseName(QString("DRIVER={SQL SERVER};"
                               "SERVER=%1;"
                               "DATABASE=%2;"
                               "UID=%3;"
                               "PWD=%4;")
                       .arg(tc.DS.ip)
                       .arg(tc.DS.dbName)
                       .arg(tc.DS.loginName)
                       .arg(tc.DS.password));
    db->setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=2;SQL_ATTR_CONNECTION_TIMEOUT=2");

    int sCanDO = -1;
    sCanDO = readPingStatus();

    if(sCanDO == -1)
    {
        emit sqlError(2);
        delete db;
        return -1;
    }

    if(!db->open())
    {
        emit sqlError(1);
        delete db;
        return -1;
    }

    //写TestData表
    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QSqlQuery q(*db);
    int flag = 0;
    QString sn;
    if(list.at(0) == NULL)
        sn = "";
    else
        sn = list.at(0);

    if(sn == "")
    {
        sn = barCode;
    }else{
        barCode = sn;
    }

    q.prepare("insert into UDT_QM_SafetyCheck (BarCode,TestTime,TestGroup,Operator,Result,"
              "TestItem1,Item1Tag1,Item1Data1,Item1Tag2,Item1Data2,Item1Tag3,Item1Data3,Item1Mode,Item1Result,"
              "TestTiem2,Item2Tag1,Item2Data1,Item2Tag2,Item2Data2,Item2Tag3,Item2Data3,Item2Mode,Item2Result,"
              "TestTiem3,Item3Tag1,Item3Data1,Item3Tag2,Item3Data2,Item3Tag3,Item3Data3,Item3Mode,Item3Result,"
              "TestTiem4,Item4Tag1,Item4Data1,Item4Tag2,Item4Data2,Item4Tag3,Item4Data3,Item4Mode,Item4Result,MachineNum)"
              " VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9,:v10,:v11,:v12,:v13,:v14,:v15,:v16,:v17,:v18,:v19,:v20"
              ",:v21,:v22,:v23,:v24,:v25,:v26,:v27,:v28,:v29,:v30,:v31,:v32,:v33,:v34,:v35,:v36,:v37,:v38,:v39,:v40"
              ",:v41,:v42);");

    q.bindValue(":v1",sn);
    q.bindValue(":v2",saveTime);
    q.bindValue(":v3",list.at(1));
    q.bindValue(":v4",list.at(2));

    if(list.at(3) == "0")
        q.bindValue(":v5",tr("不合格"));
    else
        q.bindValue(":v5",tr("合格"));

    for(int i=0;i<datalist.size();i++)
    {
        if(datalist.at(i).model == 1)
        {
            flag = 1;
            q.bindValue(":v6",tr("交耐"));
            q.bindValue(":v7",rangeSets[0].unit[0]);
            q.bindValue(":v8",QString::number(datalist.at(i).voltage,10,rangeSets[0].prec[0]));
            q.bindValue(":v9",rangeSets[0].unit[1]);
            q.bindValue(":v10",QString::number(datalist.at(i).current,10,rangeSets[0].prec[1]));
            q.bindValue(":v11","");
            q.bindValue(":v12","0.0");
            q.bindValue(":v13","");
            if(datalist.at(i).flag1 == 1)
            {
                q.bindValue(":v14",tr("合格"));
            }else{
                q.bindValue(":v14",tr("不合格"));
            }
        }
    }

    if(flag == 0)
    {
        q.bindValue(":v6",tr("无(未测试)"));
        q.bindValue(":v7","");
        q.bindValue(":v8","0.0");
        q.bindValue(":v9","");
        q.bindValue(":v10","0.0");
        q.bindValue(":v11","");
        q.bindValue(":v12","0.0");
        q.bindValue(":v13","——");
        q.bindValue(":v14","——");
    }else{
        flag = 0;
    }

    for(int i=0;i<datalist.size();i++)
    {
        if(datalist.at(i).model == 3)
        {
            flag = 1;
            q.bindValue(":v15",tr("绝缘"));
            q.bindValue(":v16",rangeSets[2].unit[0]);
            q.bindValue(":v17",QString::number(datalist.at(i).voltage,10,rangeSets[2].prec[0]));
            q.bindValue(":v18",rangeSets[2].unit[1]);
            q.bindValue(":v19",QString::number(datalist.at(i).resistance,10,rangeSets[2].prec[1]));
            q.bindValue(":v20","");
            q.bindValue(":v21","0.0");
            q.bindValue(":v22","");
            if(datalist.at(i).flag1 == 1)
            {
                q.bindValue(":v23",tr("合格"));
            }else{
                q.bindValue(":v23",tr("不合格"));
            }
        }
    }

    if(flag == 0)
    {
        q.bindValue(":v15",tr("无(未测试)"));
        q.bindValue(":v16","");
        q.bindValue(":v17","0.0");
        q.bindValue(":v18","");
        q.bindValue(":v19","0.0");
        q.bindValue(":v20","");
        q.bindValue(":v21","0.0");
        q.bindValue(":v22","——");
        q.bindValue(":v23","——");
    }else{
        flag = 0;
    }

    for(int i=0;i<datalist.size();i++)
    {
        if(datalist.at(i).model == 4)
        {
            flag = 1;
            q.bindValue(":v24",tr("接地"));
            q.bindValue(":v25",rangeSets[3].unit[3]);
            q.bindValue(":v26",QString::number(datalist.at(i).current,10,rangeSets[3].prec[0]));
            q.bindValue(":v27",rangeSets[3].unit[1]);
            q.bindValue(":v28",QString::number(datalist.at(i).resistance,10,rangeSets[3].prec[1]));
            q.bindValue(":v29","");
            q.bindValue(":v30","0.0");
            q.bindValue(":v31","");
            if(datalist.at(i).flag1 == 1)
            {
                q.bindValue(":v32",tr("合格"));
            }else{
                q.bindValue(":v32",tr("不合格"));
            }
        }
    }

    if(flag == 0)
    {
        q.bindValue(":v24",tr("无(未测试)"));
        q.bindValue(":v25","");
        q.bindValue(":v26","0.0");
        q.bindValue(":v27","");
        q.bindValue(":v28","0.0");
        q.bindValue(":v29","");
        q.bindValue(":v30","0.0");
        q.bindValue(":v31","——");
        q.bindValue(":v32","——");
    }else{
        flag = 0;
    }

    for(int i=0;i<datalist.size();i++)
    {
        if(datalist.at(i).model == 5)
        {
            flag = 1;
            QString cd = list.at(4+datalist.at(i).step);
            QStringList cdList = cd.split(",");

            q.bindValue(":v33",tr("泄漏"));
            q.bindValue(":v34",rangeSets[4].unit[0]);
            q.bindValue(":v35",QString::number(datalist.at(i).voltage,10,rangeSets[4].prec[0]));
            q.bindValue(":v36",rangeSets[4].unit[1]);
            q.bindValue(":v37",QString::number(datalist.at(i).current,10,rangeSets[4].prec[1]));
            q.bindValue(":v38","");
            q.bindValue(":v39","0.0");

            if(!cdList.isEmpty())
            {
                if(cdList.at(8) == "0")
                    q.bindValue(":v40","静态");
                else
                    q.bindValue(":v40","动态");
            }else{
                q.bindValue(":v40","");
            }

            if(datalist.at(i).flag1 == 1)
            {
                q.bindValue(":v41",tr("合格"));
            }else{
                q.bindValue(":v41",tr("不合格"));
            }
        }
    }

    if(flag == 0)
    {
        q.bindValue(":v33",tr("无(未测试)"));
        q.bindValue(":v34","");
        q.bindValue(":v35","0.0");
        q.bindValue(":v36","");
        q.bindValue(":v37","0.0");
        q.bindValue(":v38","");
        q.bindValue(":v39","0.0");
        q.bindValue(":v40","——");
        q.bindValue(":v41","——");
    }else{
        flag = 0;
    }

    q.bindValue(":v42",tc.DS.machineNum);
    bool ok = q.exec();
    int reValue = 0;
    if(ok == false)
    {
       reValue = -1;
    }

    db->close();
    delete db;
    return reValue;
}

int sqlOperate::testDataToLocalSQL(QStringList list,QList<testDataStr> datalist)
{
    int resend = resendData();
    if(resend == -2)
    {
        return -1;
    }

    QSqlDatabase *db;
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","db"));
    db->setDatabaseName(QString("DRIVER={SQL SERVER};"
                               "SERVER=%1;"
                               "DATABASE=%2;"
                               "UID=%3;"
                               "PWD=%4;")
                       .arg(tc.DS.ip)
                       .arg(tc.DS.dbName)
                       .arg(tc.DS.loginName)
                       .arg(tc.DS.password));
    db->setConnectOptions("SQL_ATTR_LOGIN_TIMEOUT=2;SQL_ATTR_CONNECTION_TIMEOUT=2");

    int sCanDO = -1;
    sCanDO = readPingStatus();

    if(sCanDO == -1)
    {
        emit sqlError(2);
        delete db;
        return -1;
    }

    if(!db->open())
    {
        emit sqlError(1);
        delete db;
        return -1;
    }

    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //写TestSummary表
    QSqlQuery q(*db);
    QString sql = QString("insert into TestSummary (barCode,programName,operator,result,recordTime,machineNum) VALUES('%1','%2','%3','%4','%5','%6');")
            .arg(list.at(0)).arg(list.at(1)).arg(list.at(2)).arg(list.at(3)).arg(saveTime).arg(tc.DS.machineNum);
    bool ok = q.exec(sql);

    //写TestData表
    for(int i=0;i<datalist.size();i++)
    {
        q.clear();
        QString name = "";
        int result = 0;
        QStringList data;
        switch(datalist.at(i).model)
        {
            case 0:
                name = tr("无效");
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                break;
            case 1:
                name = tr("交耐");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[0].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[0].prec[1]);
                data<<"";
                data<<rangeSets[0].unit[0];
                data<<rangeSets[0].unit[1];
                data<<"";
                break;
            case 2:
                name = tr("直耐");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[1].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[1].prec[1]);
                data<<"";
                data<<rangeSets[1].unit[0];
                data<<rangeSets[1].unit[1];
                data<<"";
                break;
            case 3:
                name = tr("绝缘");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[2].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[2].prec[1]);
                data<<"";
                data<<rangeSets[2].unit[0];
                data<<rangeSets[2].unit[1];
                data<<"";
                break;
            case 4:
                name = tr("接地");
                data<<QString::number(datalist.at(i).current,10,rangeSets[3].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[3].prec[1]);
                data<<"";
                data<<rangeSets[3].unit[3];
                data<<rangeSets[3].unit[1];
                data<<"";
                break;
            case 5:
                name = tr("泄漏");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[4].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[4].prec[1]);
                data<<"";
                data<<rangeSets[4].unit[0];
                data<<rangeSets[4].unit[1];
                data<<"";
                break;
            case 6:
                name = tr("功率");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[5].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[5].prec[3]);
                data<<QString::number(datalist.at(i).power,10,rangeSets[5].prec[1]);
                data<<rangeSets[5].unit[0];
                data<<rangeSets[5].unit[3];
                data<<rangeSets[5].unit[1];
                break;
            case 7:
                name = tr("低启");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[6].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[6].prec[1]);
                data<<QString::number(datalist.at(i).power,10,rangeSets[6].prec[3]);
                data<<rangeSets[6].unit[0];
                data<<rangeSets[6].unit[1];
                data<<rangeSets[6].unit[3];
                break;
            case 8:
                name = tr("开短");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[7].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[7].prec[1]);
                data<<"";
                data<<rangeSets[7].unit[0];
                data<<rangeSets[7].unit[1];
                data<<"";
                break;
        }

        if(datalist.at(i).flag1 == 1)
        {
            result = 1;
        }else{
            result = 0;
        }

        QString resultFlag = QString("%1,%2,%3,%4,%5,%6").arg(datalist.at(i).flag3).arg(datalist.at(i).flag4)
                .arg(datalist.at(i).flag5).arg(datalist.at(i).flag6).arg(datalist.at(i).flag7).arg(datalist.at(i).flag8);

        QString cd = list.at(4+datalist.at(i).step);
        QStringList cdList = cd.split(",");

        QString sn;
        if(list.at(0) == NULL)
            sn = "";
        else
            sn = list.at(0);

        q.prepare("insert into TestData (barCode,sequence,itemID,itemName,condition1,conditionUnit1,"
                      "condition2,conditionUnit2,condition3,conditionUnit3,condition4,conditionUnit4,"
                      "StaticDynamic,result,resultFlag,result1Data,result2Data,result3Data,result1Unit,"
                  "result2Unit,result3Unit,recordTime,machineNum) VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9"
                  ",:v10,:v11,:v12,:v13,:v14,:v15,:v16,:v17,:v18,:v19,:v20,:v21,:v22,:v23);");

        q.bindValue(":v1",sn);
        q.bindValue(":v2",i+1);
        q.bindValue(":v3",datalist.at(i).model);
        q.bindValue(":v4",name);
        q.bindValue(":v5",cdList.at(0));
        q.bindValue(":v6",cdList.at(1));
        q.bindValue(":v7",cdList.at(2));
        q.bindValue(":v8",cdList.at(3));
        q.bindValue(":v9",cdList.at(4));
        q.bindValue(":v10",cdList.at(5));
        q.bindValue(":v11",cdList.at(6));
        q.bindValue(":v12",cdList.at(7));
        q.bindValue(":v13",cdList.at(8));
        q.bindValue(":v14",result);
        q.bindValue(":v15",resultFlag);
        q.bindValue(":v16",data.at(0));
        q.bindValue(":v17",data.at(1));
        q.bindValue(":v18",data.at(2));
        q.bindValue(":v19",data.at(3));
        q.bindValue(":v20",data.at(4));
        q.bindValue(":v21",data.at(5));
        q.bindValue(":v22",saveTime);
        q.bindValue(":v23",tc.DS.machineNum);
        ok = q.exec();
    }

    db->close();
    delete db;
    return 0;
}

//int sqlOperate::getRemoteOpen(int value)
//{
//    sCanDO = value;
//}

int sqlOperate::readPingStatus()
{
    int exitCode,value;

    QString ip = tc.DS.ip;

    QString cmd = "ping " + ip + " -n 1 -w 100";  //windows下的格式
    exitCode = QProcess::execute(cmd);

    if(0 == exitCode)
    {
        value = 1;
    }else{
        value = -1;
    }
    return value;
}

void sqlOperate::sqlOperate::write(QStringList list,QList<testDataStr> datalist)
{
    qDebug()<<"write2"<<list;
    if(list.isEmpty() || datalist.isEmpty())
        return;

    emit sendsDo(1);

    qDebug()<<"write3"<<list;
    QDate time = QDate::currentDate();
    QString fileName = time.toString("yyyy-MM-dd")+".mdb";
    QString sDbNm = QCoreApplication::applicationDirPath()+"/data/"+fileName;

//    if(!QFile::exists(sDbNm))
//    {
//        QFile::copy(QCoreApplication::applicationDirPath()+"/db/TestData.mdb",sDbNm);
//        tc.iTotal = 0;
//        tc.iPass = 0;
//        tc.iNum[0] = 0;
//        tc.iNum[1] = 0;
//        tc.iNum[2] = 0;
//        tc.iNum[3] = 0;
//        tc.iNum[4] = 0;
//        tc.iNum[5] = 0;
//        tc.iNum[6] = 0;
//        tc.iNum[7] = 0;
//        tc.iNGNum[0] = 0;
//        tc.iNGNum[1] = 0;
//        tc.iNGNum[2] = 0;
//        tc.iNGNum[3] = 0;
//        tc.iNGNum[4] = 0;
//        tc.iNGNum[5] = 0;
//        tc.iNGNum[6] = 0;
//        tc.iNGNum[7] = 0;
//    }

//    if(!QFile::exists(sDbNm))
//    {
//        emit sendsDo(0);
//        return;
//    }

//    int sCanDO = -1;
//    if(tc.STD.upload == 1)
//    {
//        sCanDO = readPingStatus();
//    }

    int saveOk = 1;
    if(tc.STD.upload == 1)
    {
        QStringList ls;
        ls = list;
        QList<testDataStr> tdList;

        for(int i=0;i<datalist.count();i++)
        {
            tdList<<datalist.at(i);
        }

        if(tc.STD.uploadType == 1)
        {
            saveOk = dataToHttp(ls,tdList);
        }else if(tc.STD.uploadType == 2){
            saveOk = testDataToLocalSQLAIP(ls,tdList);
        }else if(tc.STD.uploadType == 3){
            saveOk = testDataToLocalSQLIDI(ls,tdList);
        }else{
            saveOk = testDataToLocalSQL(ls,tdList);
        }
    }else if(tc.STD.upload == 1){
        saveOk = -1;
    }

    QSqlDatabase *db;
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","dbAccess"));
    QString dsn = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNm);//连接字符串
    db->setDatabaseName(dsn);
    db->setUserName("");     //设置登陆数据库的用户名
    db->setPassword("");     //设置密码
    bool ok = db->open();

    if(!ok)
    {
        db->removeDatabase("dbAccess");
        delete db;
        emit sendsDo(0);
        return;
    }

    int a[8] = {0,0,0,0,0,0,0,0};
    int b[8] = {0,0,0,0,0,0,0,0};
    QString saveTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //写TestSummary表
    QSqlQuery q(*db);
    int rec = saveOk;

    QString sql = QString("insert into TestSummary (barCode,programName,operator,result,recordTime,num) VALUES('%1','%2','%3','%4','%5',%6);")
            .arg(list.at(0)).arg(list.at(1)).arg(list.at(2)).arg(list.at(3)).arg(saveTime).arg(rec);    
    ok = q.exec(sql);

    //写TestData表
    for(int i=0;i<datalist.size();i++)
    {
        q.clear();
        QString name = "";
        int result = 0;
        QStringList data;
        switch(datalist.at(i).model)
        {
            case 0:
                name = tr("无效");
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                data<<"";
                break;
            case 1:
                name = tr("交耐");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[0].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[0].prec[1]);
                data<<"";
                data<<rangeSets[0].unit[0];
                data<<rangeSets[0].unit[1];
                data<<"";
                break;
            case 2:
                name = tr("直耐");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[1].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[1].prec[1]);
                data<<"";
                data<<rangeSets[1].unit[0];
                data<<rangeSets[1].unit[1];
                data<<"";
                break;
            case 3:
                name = tr("绝缘");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[2].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[2].prec[1]);
                data<<"";
                data<<rangeSets[2].unit[0];
                data<<rangeSets[2].unit[1];
                data<<"";
                break;
            case 4:
                name = tr("接地");
                data<<QString::number(datalist.at(i).current,10,rangeSets[3].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[3].prec[1]);
                data<<"";
                data<<rangeSets[3].unit[3];
                data<<rangeSets[3].unit[1];
                data<<"";
                break;
            case 5:
                name = tr("泄漏");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[4].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[4].prec[1]);
                if(tc.phase == 1)
                    data<<QString::number(datalist.at(i).pCurrent,10,0);
                else
                    data<<"";
                data<<rangeSets[4].unit[0];
                data<<rangeSets[4].unit[1];
                if(tc.phase == 1)
                    data<<"W";
                else
                    data<<"";
                break;
            case 6:
                name = tr("功率");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[5].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[5].prec[3]);
                data<<QString::number(datalist.at(i).power,10,rangeSets[5].prec[1]);
                data<<rangeSets[5].unit[0];
                data<<rangeSets[5].unit[3];
                data<<rangeSets[5].unit[1];
                break;
            case 7:
                name = tr("低启");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[6].prec[0]);
                data<<QString::number(datalist.at(i).current,10,rangeSets[6].prec[1]);
                data<<QString::number(datalist.at(i).power,10,rangeSets[6].prec[3]);
                data<<rangeSets[6].unit[0];
                data<<rangeSets[6].unit[1];
                data<<rangeSets[6].unit[3];
                break;
            case 8:
                name = tr("开短");
                data<<QString::number(datalist.at(i).voltage,10,rangeSets[7].prec[0]);
                data<<QString::number(datalist.at(i).resistance,10,rangeSets[7].prec[1]);
                data<<"";
                data<<rangeSets[7].unit[0];
                data<<rangeSets[7].unit[1];
                data<<"";
                break;
        }

        if(datalist.at(i).flag1 == 1)
        {
            result = 1;
        }else{
            result = 0;
        }

        QString resultFlag = QString("%1,%2,%3,%4,%5,%6").arg(datalist.at(i).flag3).arg(datalist.at(i).flag4)
                .arg(datalist.at(i).flag5).arg(datalist.at(i).flag6).arg(datalist.at(i).flag7).arg(datalist.at(i).flag8);

        QString cd = list.at(4+datalist.at(i).step);        
        QStringList cdList = cd.split(",");
        QString state = QString("%1|%2").arg(tc.phase).arg(cdList.at(8));

        q.prepare("insert into TestData (barCode,sequence,itemID,itemName,condition1,conditionUnit1,"
                      "condition2,conditionUnit2,condition3,conditionUnit3,condition4,conditionUnit4,"
                      "StaticDynamic,result,resultFlag,result1Data,result2Data,result3Data,result1Unit,"
                  "result2Unit,result3Unit,recordTime,num) VALUES(:v1,:v2,:v3,:v4,:v5,:v6,:v7,:v8,:v9"
                  ",:v10,:v11,:v12,:v13,:v14,:v15,:v16,:v17,:v18,:v19,:v20,:v21,:v22,:v23);");

        q.bindValue(":v1",list.at(0));
        q.bindValue(":v2",i+1);
        q.bindValue(":v3",datalist.at(i).model);
        q.bindValue(":v4",name);
        q.bindValue(":v5",cdList.at(0));
        q.bindValue(":v6",cdList.at(1));
        q.bindValue(":v7",cdList.at(2));
        q.bindValue(":v8",cdList.at(3));
        q.bindValue(":v9",cdList.at(4));
        q.bindValue(":v10",cdList.at(5));
        q.bindValue(":v11",cdList.at(6));
        q.bindValue(":v12",cdList.at(7));
        q.bindValue(":v13",state);
        q.bindValue(":v14",result);
        q.bindValue(":v15",resultFlag);
        q.bindValue(":v16",data.at(0));
        q.bindValue(":v17",data.at(1));
        q.bindValue(":v18",data.at(2));
        q.bindValue(":v19",data.at(3));
        q.bindValue(":v20",data.at(4));
        q.bindValue(":v21",data.at(5));
        q.bindValue(":v22",saveTime);
        q.bindValue(":v23",rec);
        ok = q.exec();
    }
    q.clear();
    if(tc.STD.upload == 1 && saveOk == -1)
    {
        QSqlDatabase *dbUP;
        dbUP = new QSqlDatabase(QSqlDatabase::addDatabase("QODBC","dbSaveOk"));
        QString sDbNmUP = QCoreApplication::applicationDirPath()+"/db/SysSet.mdb";
        QString dsnUP = QString("DRIVER={Microsoft Access Driver (*.mdb, *.accdb)}; FIL={MS Access};DBQ=%1;").arg(sDbNmUP);//连接字符串
        dbUP->setDatabaseName(dsnUP);
        dbUP->setUserName("");     //设置登陆数据库的用户名
        dbUP->setPassword("");     //设置密码
        bool okUP = dbUP->open();

        if(!okUP)
        {
            dbUP->removeDatabase("dbSaveOk");
            delete dbUP;
            db->removeDatabase("dbAccess");
            delete db;
            emit sendsDo(0);
            return;
        }

        QSqlQuery qUP(*dbUP);

        okUP = qUP.exec(QString("select count(*) from resendList where fileName='%1';").arg(fileName));
        qUP.next();
        int num = qUP.value(0).toInt();
        qUP.clear();

        if(num < 1)
        {
            qUP.exec(QString("insert into resendList (fileName) VALUES('%1');").arg(fileName));
            qUP.clear();
        }
        dbUP->removeDatabase("dbSaveOk");
        dbUP->close();
        delete dbUP;
    }

    db->removeDatabase("dbAccess");
    db->close();
    delete db;
    emit sendsDo(0);

    if(tc.txtEnable == 1)
    {
        QString sn = list.at(0);
        if(sn.size() == 0)
            sn = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");

        QString fileName = QString("%1/%2/%3.txt").arg(tc.txtFile)
                .arg(saveTime.mid(0,10))
                .arg(sn);
        QFile file(fileName);
        file.open(QIODevice::WriteOnly | QIODevice::Text );
        QTextStream in(&file);

        in<<saveTime.mid(0,10)<<"\n";
        in<<saveTime.mid(11,8)<<"\n";
        if(list.at(3) == "0")
            in<<"FAIL"<<"\n";
        else
            in<<"PASS"<<"\n";

        for(int i=0;i<datalist.size();i++)
        {
            in<<QString("%1\t").arg(i+1);

            QString name1 = NULL;
            QString tmp = NULL;
            QString cd1 = list.at(4+datalist.at(i).step);
            QStringList cdList1 = cd1.split(",");

            switch(datalist.at(i).model)
            {
                case 0:
                    in<<tr("无效")<<"\n";
                    break;
                case 1:
                    name1 = tr("交耐");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[0].unit[0]<<"\t";
                    in<<QString::number(datalist.at(i).voltage,10,rangeSets[0].prec[0])<<rangeSets[0].unit[0]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[0].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).current,10,rangeSets[0].prec[1])<<rangeSets[0].unit[1]<<"\t";

                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
                case 2:
                    name1 = tr("直耐");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[1].unit[0]<<"\t";
                    in<<QString::number(datalist.at(i).voltage,10,rangeSets[1].prec[0])<<rangeSets[1].unit[0]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[1].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).current,10,rangeSets[1].prec[1])<<rangeSets[1].unit[1]<<"\t";

                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
                case 3:
                    name1 = tr("绝缘");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[2].unit[0]<<"\t";
                    in<<QString::number(datalist.at(i).voltage,10,rangeSets[2].prec[0])<<rangeSets[2].unit[0]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[2].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).resistance,10,rangeSets[2].prec[1])<<rangeSets[2].unit[1]<<"\t";

                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
                case 4:
                    name1 = tr("接地");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[3].unit[3]<<"\t";
                    in<<QString::number(datalist.at(i).current,10,rangeSets[3].prec[0])<<rangeSets[3].unit[3]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[3].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).resistance,10,rangeSets[3].prec[1])<<rangeSets[3].unit[1]<<"\t";

                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
                case 5:
                    name1 = tr("泄漏");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[4].unit[0]<<"\t";
                    in<<QString::number(datalist.at(i).voltage,10,rangeSets[4].prec[0])<<rangeSets[4].unit[0]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[4].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).current,10,rangeSets[3].prec[1])<<rangeSets[4].unit[1]<<"\t";

                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
                case 6:
                    name1 = tr("功率");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[5].unit[0]<<"\t";
                    in<<QString::number(datalist.at(i).voltage,10,rangeSets[5].prec[0])<<rangeSets[5].unit[0]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[5].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).power,10,rangeSets[5].prec[1])<<rangeSets[5].unit[1]<<"\t";
                    in<<QString::number(datalist.at(i).current,10,rangeSets[5].prec[3])<<rangeSets[5].unit[3]<<"\t";
                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
                case 7:
                    name1 = tr("低启");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[5].unit[0]<<"\t";
                    in<<QString::number(datalist.at(i).voltage,10,rangeSets[6].prec[0])<<rangeSets[6].unit[0]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[6].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).current,10,rangeSets[6].prec[1])<<rangeSets[6].unit[1]<<"\t";
                    in<<QString::number(datalist.at(i).power,10,rangeSets[6].prec[3])<<rangeSets[6].unit[3]<<"\t";
                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
                case 8:
                    name1 = tr("开短");
                    in<<name1<<"\t";

                    in<<cdList1.at(0)<<rangeSets[7].unit[0]<<"\t";
                    in<<QString::number(datalist.at(i).voltage,10,rangeSets[7].prec[0])<<rangeSets[7].unit[0]<<"\t";

                    in<<QString("%1 --- %2%3").arg(cdList1.at(2)).arg(cdList1.at(4)).arg(rangeSets[7].unit[1])<<"\t";
                    in<<QString::number(datalist.at(i).resistance,10,rangeSets[7].prec[1])<<rangeSets[7].unit[1]<<"\t";

                    in<<cdList1.at(6)<<cdList1.at(7)<<"\t"<<"0.0"<<cdList1.at(7)<<"\t";

                    if(datalist.at(i).flag1 == 1)
                    {
                        tmp = tr("合格");
                    }else{
                        tmp = tr("不合格");
                    }
                    in<<tmp<<"\n";
                    break;
            }
        }

        QString t = "-------------------------------------------------------------------------------------------------------------";
        in<<t;
        file.flush();
        file.close();
        file.deleteLater();
    }
}
