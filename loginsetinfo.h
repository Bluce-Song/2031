#ifndef LOGINSETINFO_H
#define LOGINSETINFO_H
#include <QString>
#include <QList>

struct range_Item{
    double def[8];
    double minDefault[8];
    double min[8];
    double maxDefault[8];
    double max[8];
    int precDefault[8];
    int prec[8];
    QString unit[8];

    int id;
    int active;
};

struct registeStr{
    int regState;
    QString userName;
    QString makeDate;
    QString expiry;
    QString cpuID;
    QString keyID;
    QString version;
    QString Mode;
};

struct groupSetItem{
    QString itemName;
    double data[8];
    QString unit[8];
    int prec[8];
    int id;
    int state;
};

struct groupConfig{
    int groupNum;
    int itemNum;
    QList<int> itemId;
    int frequency;
    int NG;
    int invalid;
    int gnd;
    int machineModel;
    int phase;
    int hotVoltage;
    int MD;
    int ACDC;
    float upTime;
    float downTime;

    QString groupName;
    QString IDCode;
    QString saveTime;
    QStringList condition;
    QList<groupSetItem> items;
};

struct serialPortSet{
    int port;
    int baudRate;
    int machineAddr;
    int sPort;
    int sBaudRate;
    int sEnable;
    int pplPort;
    int pplBaudRate;
    int pplEnable;
    int cPort;
    int cBaudRate;
    int psEnable;
    int psPort1;
    int psBaudRate1;
    int psPort2;
    int psBaudRate2;
    float psCoefficient1;
    float psCoefficient2;
    float psCoefficient3;
    float psCoefficient4;
    float psMin;
    float psMax;
};

struct systemParameter{
    int leakageOffet;
    int GNDOffset;
    int ACOffset;
    int DCOffset;
    int ground;
    int alarm;
    int plc;
};

struct autoCode{
    QString prefix;
    int code;
    int lastCode;
    QString suffix;
    int length;
    int down;
    int keep;
};

struct barCodeInput{
    int startByBarCode;
    int setByBarCode;
    int barCodeLength;
    int lengthCheck;
};

struct saveTestData{
    int saveAll;
    int savePass;
    int savaFail;
    int doNotSave;
    int upload;
    int uploadType;
    int repeat;
    int repeatType;
};

struct databaseSet{
    int port;
    QString ip;
    QString dbName;
    QString machineNum;
    QString loginName;
    QString password;
};

struct printSet{
    int printPass;
    int printFail;
    int autoPrint;
    int PrintType;
    int PrintPort;
    int PrintBaudRate;
};

struct testDataStr{    
    int group;
    int step;
    int id;

    int model;
    int next;

    int flag;
    int flag1;
    int flag2;
    int flag3;
    int flag4;
    int flag5;
    int flag6;
    int flag7;
    int flag8;

    float testTime;
    float voltage;
    float current;
    float resistance;
    float power;
    float pCurrent;
    float lCurrent;
    float freq;
    float pf;
};

struct testConfig{    
    int state;
    int subState;
    int language;
    int codeMode;
    int userRights[8];
    int userLevel;
    int portOpen;
    int sPortOpen;
    int pPortOpen;
    int cPortOpen;
    int psPort1Open;
    int psPort2Open;
    int startMode;
    int step;
    int total;
    int passNum;
    int itemNum;
    int itemNumMax;
    int machineModel;
    int phase;
    int md;
    int sCanDO;
    QString ACW_I_K;
//    float psCoefficient1;
//    float psCoefficient2;
//    float psCoefficient3;
//    float psCoefficient4;
//    int lltbb;
    int tcpScanEnable;
    int tcpScanPort;    

    QString iDate;
    int iTotal;
    int iPass;
    int iNum[8] = {0,0,0,0,0,0,0,0};
    int iNGNum[8] = {0,0,0,0,0,0,0,0};

    QString userName;
    QString lastUser;
    QString currentGroup;
    QString lastGroup;
    QString barCode;
    QString partsCode;
    QString version;

    QString auxiliary1;
    QString auxiliary2;
    QString auxiliary3;

    int txtEnable;
    QString txtFile;

    QString url_1;
    QString url_2;
    QString url_3;

    QString tcpScanIP;

    registeStr REG;
    serialPortSet SPS;
    autoCode AC;
    barCodeInput BI;
    saveTestData STD;
    databaseSet DS;
    printSet PS;
    systemParameter SP;
};
#endif // LOGINSETINFO_H

