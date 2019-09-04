#ifndef PARAMETERFILE_H
#define PARAMETERFILE_H

#include <QWidget>
#include <QSettings>
#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
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

namespace Ui {
class parameterFile;
}

class parameterFile : public QWidget
{
    Q_OBJECT

public:
    explicit parameterFile(QWidget *parent = 0);
    ~parameterFile();

private:
    Ui::parameterFile *ui;
    void readParameter();

private slots:
    void checkAll();
    void deleteFile();
    void importFiles();
    void exitFile();
    void chooseFile(QModelIndex index);
};

#endif // PARAMETERFILE_H
