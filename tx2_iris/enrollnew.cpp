#include "enrollnew.h"
#include "ui_enrollnew.h"

enrollNew::enrollNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::enrollNew)
{
    ui->setupUi(this);
}

enrollNew::~enrollNew()
{
    delete ui;
}
