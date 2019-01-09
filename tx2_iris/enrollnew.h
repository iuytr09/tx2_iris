#ifndef ENROLLNEW_H
#define ENROLLNEW_H

#include <QDialog>

namespace Ui {
class enrollNew;
}

class enrollNew : public QDialog
{
    Q_OBJECT

public:
    explicit enrollNew(QWidget *parent = 0);
    ~enrollNew();

private:
    Ui::enrollNew *ui;
};

#endif // ENROLLNEW_H
