/*****************************************************************************
** 文 件 名：enroll.h
** 主 要 类：Enroll
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   注册模块
** 主要模块说明: 注册类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef ENROLL_H
#define ENROLL_H

#include <QtWidgets/QDialog>
#include <QtCore/QTextCodec>
#include <QElapsedTimer>
#include <QPainter>
#include "dialogenroll.h"

#include <mutex>
#include <thread>
#include <condition_variable>
#include "interaction.h"

namespace Ui {
class Enroll;
}

class IRIS_Algorith;
//绘图事件类
class ImagePaintEvent : public QObject
{
    Q_OBJECT
public:
    //绘制实时采集图像事件
    void RaisePaintCapImgEvent();
signals:
    //发送绘制实时采集图像信号
    void SigPaintCapImg();

};

//注册类
class Enroll : public QDialog
{
    Q_OBJECT
    
public:
    Enroll(QWidget *parent = 0);
    ~Enroll();


    //待删除
public:
    virtual void timerEvent(QTimerEvent *event);
private:
    int _captureImageTimerId;
    int _frameNum;

public:
    //设置人员信息
    void SetPersonInfo(PersonInfo *personInfomation);

private:

    //显示注册图像对应的原始图像
    void ShowOriginImg();

    
signals:
    void SigEnrollResult(bool isSucceed);

public:
    //绘制实时采集图像
    void CapImageUpdate(QImage im);
    void IrisImageUpdate(QImage im); //hong mo
private slots:

    void slotEnrollSuccess(int state);
    void on_btnEnroll_clicked();

    void on_btnEnrollReturn_clicked();

    void on_btnSaveEnrollResult_clicked();




private:
    Ui::Enroll *ui;
    DialogEnroll *_dialogEnroll;                //注册对话框
   int _distanceValue;//距离值

    bool _btnEnrollClicked;                                     //注册按钮是否被点击标志

    std::thread _capImgThread;                                  //注册模块图像显示线程
    std::mutex _capImgMutex;                                    //注册模块图像采集mutex
   bool _capImgFlag;                                           //注册模块图像采集flag
    bool _signaledFlag;


   //传进来的人员信息
    PersonInfo _personInfo;

    //人机交互类
    //static Interaction *s_enrollInteraction;

    //AlgApiCtrlParam _enrollParamStruct;//算法逻辑控制参数

    IRIS_Algorith* _pirisA;

};

#endif // ENROLL_H
