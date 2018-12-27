/*****************************************************************************
** 文 件 名：dialogenroll.h
** 主 要 类：DialogEnroll
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   注册对话框
** 主要模块说明: 注册过程中的对话框处理
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef DIALOGENROLL_H
#define DIALOGENROLL_H

#include <QtWidgets/QDialog>
#include <Common/common.h>

namespace Ui {
class DialogEnroll;
}


class DialogEnroll : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogEnroll(QWidget *parent = 0);
    ~DialogEnroll();
    //传递注册结果信息
    void XferIrisInfo(QImage resultImg,double qualityScore);

    
private slots:
   void on_btnDlgEnrollReturn_clicked();

private:
    Ui::DialogEnroll *ui;                                                //注册结果
};

#endif // DIALOGENROLL_H
