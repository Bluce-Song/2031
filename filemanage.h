#ifndef FILEMANAGE_H
#define FILEMANAGE_H

#include <QWidget>

namespace Ui {
class fileManage;
}

class fileManage : public QWidget
{
    Q_OBJECT

public:
    explicit fileManage(QWidget *parent = 0);
    ~fileManage();

private:
    Ui::fileManage *ui;
};

#endif // FILEMANAGE_H
