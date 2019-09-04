#-------------------------------------------------
#
# Project created by QtCreator 2017-05-16T09:04:06
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += serialport
QT       += core gui sql
QT       += charts
QT       += axcontainer
QT += xml network
RC_ICONS = app.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AGFirstGeneration
TEMPLATE = app
TRANSLATIONS += user_zh.ts

SOURCES += main.cpp\
    loginDialog.cpp \
    test_main.cpp \
    comand.cpp \
    range.cpp \
    editGroups.cpp \
    choosegroup.cpp \
    filemanage.cpp \
    parameter.cpp \
    testitem.cpp \
    groupitem.cpp \
    serialport.cpp \
    sqloperate.cpp \
    queryandreport.cpp \
    scanner.cpp \
    serialhfhaier.cpp \
    registe.cpp \
    qtsoap.cpp \
    serialmultiple.cpp \
    serialportch.cpp \
    serialppl.cpp \
    seriallpc.cpp \
    selfcheck.cpp \
    parameterbase.cpp \
    parameterfile.cpp \
    parameterrule.cpp \
    parameteruser.cpp \
    parameterrange.cpp \
    parametersenior.cpp \
    serialxsb.cpp \
    dustfiltration.cpp \
    testitemxsb.cpp \
    sqlremote.cpp \
    SerialWeiHuangE32.cpp

HEADERS  += \
    loginsetinfo.h \
    loginDialog.h \
    test_main.h \
    comand.h \
    range.h \
    editGroups.h \
    choosegroup.h \
    filemanage.h \
    parameter.h \
    testitem.h \
    groupitem.h \
    serialport.h \
    asynctimer.h \
    sqloperate.h \
    queryandreport.h \
    scanner.h \
    serialhfhaier.h \
    registe.h \
    qtsoap.h \
    serialmultiple.h \
    serialportch.h \
    serialppl.h \
    seriallpc.h \
    selfcheck.h \
    parameterbase.h \
    parameterfile.h \
    parameterrule.h \
    parameteruser.h \
    parameterrange.h \
    parametersenior.h \
    serialxsb.h \
    dustfiltration.h \
    testitemxsb.h \
    sqlremote.h \
    SerialWeiHuangE32.h

FORMS    += login.ui \
    test_main.ui \
    loginDialog.ui \
    sysConfig.ui \
    encodingRules.ui \
    userManage.ui \
    range.ui \
    editGroups.ui \
    choosegroup.ui \
    filemanage.ui \
    parameter.ui \
    queryandreport.ui \
    registe.ui \
    selfcheck.ui \
    parameterbase.ui \
    parameterfile.ui \
    parameterrule.ui \
    parameteruser.ui \
    parameterrange.ui \
    parametersenior.ui

RESOURCES += \
    image.qrc
