/*****************************************************************************
** 文 件 名：datamanage.h
** 主 要 类：DataManage
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   数据管理
** 主要模块说明: 人员信息， 虹膜， 识别记录的导入导出
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef DATAMANAGE_H
#define DATAMANAGE_H

#include <QtWidgets/QDialog>


namespace Ui {
class DataManage;
}

class DataManage : public QDialog
{
    Q_OBJECT
    
public:
    explicit DataManage(QWidget *parent = 0);
    ~DataManage();

protected:
    QString getCheckedItems();

signals:
    //TO DO
    void sigGoBack();
    void sigSuccess(bool);
    void sigError(bool);
    void sigAbort();

protected slots:
    void slotPersonInfoSelectChanged();
    void slotImport();
    void slotExport();
    
private:
    Ui::DataManage *ui;
};

#endif // DATAMANAGE_H
