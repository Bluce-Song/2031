#ifndef CV_H
#define CV_H

#include <QWidget>

namespace Ui {
class CV;
}

class CV : public QWidget
{
    Q_OBJECT

public:
    explicit CV(QWidget *parent = 0);
    ~CV();
    void setTestName(QString name);

private:
    Ui::CV *ui;
};

#endif // CV_H
