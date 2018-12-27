/*****************************************************************************
** 文 件 名：DialogAbout.h
** 主 要 类：DialogAbout
**
** Copyright (c) shanggushijue
**
** 创 建 人：刘中昌
** 创建时间：2018-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   关于界面
** 主要模块说明: 显示版本信息、公司信息等
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QtWidgets/QDialog>

namespace Ui {
class DialogAbout;
}

class DialogAbout : public QDialog
{
    Q_OBJECT

    
public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();
signals:
    void sigGoBack();

//protected:
//    void focusOutEvent(QFocusEvent*);
    
private:
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
