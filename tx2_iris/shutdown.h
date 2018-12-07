/*****************************************************************************
** 文 件 名：shutdown.h
** 主 要 类：Shutdown
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   关机操作界面
** 主要模块说明: 关机， 取消关机， 重启
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <QtWidgets/QDialog>

namespace Ui {
class Shutdown;
}

class Shutdown : public QDialog
{
    Q_OBJECT
    
public:
    explicit Shutdown(QWidget *parent = 0);
    ~Shutdown();

signals:
    void sigGoBack();

protected slots:
    void slotShutdown();
    void slotReboot();

private:
    Ui::Shutdown *ui;
};

#endif // SHUTDOWN_H
