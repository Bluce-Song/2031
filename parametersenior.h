#ifndef PARAMETERSENIOR_H
#define PARAMETERSENIOR_H

#include <QWidget>
#include "loginsetinfo.h"

extern testConfig tc;
namespace Ui {
class parameterSenior;
}

class parameterSenior : public QWidget
{
    Q_OBJECT

public:
    explicit parameterSenior(QWidget *parent = 0);
    ~parameterSenior();

private:
    Ui::parameterSenior *ui;

    void readParameter();
private slots:
    void saveSenior();
    void exitSenior();
};

#endif // PARAMETERSENIOR_H
