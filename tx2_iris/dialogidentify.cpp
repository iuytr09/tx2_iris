
/*****************************************************************************
** 文 件 名：Identity.cpp
** 主 要 类：Identity
**
** Copyright (c) shanggushijue
**
** 创 建 人：刘中昌
** 创建时间：2018-10-13
**
** 修 改 人：
** 修改时间：
** 描  述:   识别对话框
** 主要模块说明: 识别界面的显示
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <unistd.h>
#include <iostream>
#include "dialogidentify.h"
#include "ui_dialogidentify.h"
#include "time.h"
#include "QDateTime"
#include "QTime"
#include <QtCore>
#include "QThread"
#include <sys/time.h>
#include <QMessageBox>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;




DialogIdentify::DialogIdentify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogIdentify)
{
    ui->setupUi(this);

    /**********界面时间实时显示*****************/
    //获取系统现在的时间
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");//设置系统时间显示格式
    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime);


//    //新建定时器 关联当前时间
//    this->startTimer(1000);
////    connect(&identEvent,SIGNAL(sigResult(long,int)),this,SLOT(IdentResult(long,int)));
////    connect(&identEvent,SIGNAL(sigLoginResult(long,int)),this,SLOT(LoginIdentResult(long,int)));

    //设置界面背景
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/背景.jpg")));
    ui->widgetInfo->setPalette(palette);

    _isClose = false;
    _isLoginIdent = false;

//    _identInteraction = Interaction::GetInteractionInstance();
//    _irisClearFlag = LAndRImgBlur;
    _identResultState = ContinueState;

    //新建定时器
    _timerStartIdent = new QTimer();
   // 关联定时器计满信号和相应的槽函数
   // connect(_timerStartIdent,SIGNAL(timeout()),this,SLOT(StartAttendIdent()));

    //启动异步识别识别
    //StartAttendIdent();
}

DialogIdentify::~DialogIdentify()
{
    _exitIdentThread = false;

    delete _timerStartIdent;
    delete ui;
}


/*****************************************************************************
*                        开始识别
*  函 数 名： StartIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartAttendIdent()
{
    _identResultState = UnKnownState;
    _timerStartIdent->stop();

    ui->labIsLogin->setText(QString::fromUtf8("考勤识别"));

    _isLoginIdent = false;

    if(error!=0)
    {
        std::cout<<"开始识别失败！"<<std::endl;
       // QMessageBox::information(this, QString::fromUtf8("识别"), QString::fromUtf8("识别失败！"));
    }
    else
    {
        _isClose = false;
    }
}


/*****************************************************************************
*                        管理员登陆开始识别
*  函 数 名： StartIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartLoginIdent()
{
    ui->labIsLogin->setText(QString::fromUtf8("管理员登陆识别"));
    _isClose = false;
    _isLoginIdent =true;
}



/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：IdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::IdentResult(int flag)
{
    if(flag==-1)
    {
        ui->labStatu->setText(QString::fromUtf8("识别失败"));
        _identResultState = FailedState;
    }
    else if(flag == 0)
    {

        ui->labStatu->setText(QString::fromUtf8("识别成功！"));
        _identResultState = SuccessState;
    }
    else
    {
        _identResultState = ContinueState;

    }
}


/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：LoginIdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::LoginIdentResult(long personIndex,int flag)
{
    if(personIndex<0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别失败"));
    }
    else if(personIndex == 0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别中..."));
    }
    else
    {
        if(personIndex<_personName.size())
        {
            ui->labStatu->setText(_loginPersonName[personIndex] + QString::fromUtf8("识别成功"));
            emit sigLoginSucceed(true);
            //停止识别
            StopIdent();
            return;
        }
    }
    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);

    if(!_isClose && _isLoginIdent)
    {
        //异步识别识别
        StartLoginIdent();
    }
}

/*****************************************************************************
*                        停止识别函数
*  函 数 名：StopIdent
*  功    能：停止识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StopIdent()
{
    if(_isClose)
    {
        return;
    }
    _isClose = true;
    //int error = _identApi->StopIden();
    if(error != 0)
    {
        std::cout<<"停止识别失败！ "<<std::endl;
    }
}

/*****************************************************************************
*                        界面时间更新显示
*  函 数 名：timerEvent
*  功    能：界面时间更新显示
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-10-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::timerEvent(QTimerEvent *event)
{
    //获取系统现在的时间
    QDateTime time = QDateTime::currentDateTime();
    //设置系统时间显示格式
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
    //界面显示
    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime + QString::fromUtf8("  帧率： ") + QString::number(_count));
    _count =0;
}

/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：ImageUpdate
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::ImageUpdate(QImage im)
{
    if(!im.isNull())
    {
        ui->labelFace->setPixmap(QPixmap::fromImage(im));
        //_count ++;
    }
    else
    {
        std::cout<<"识别显示未获得图像！"<<std::endl;
    }
}


/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：ImageUpdate
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::IrisImageUpdate(QImage im)
{
    if(!im.isNull())
    {
        ui->labEye->setPixmap(QPixmap::fromImage(im));
        _count ++;
    }
    else
    {
        std::cout<<"识别显示未获得图像！"<<std::endl;
    }
}

