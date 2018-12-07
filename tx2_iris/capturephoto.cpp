/*****************************************************************************
** 文 件 名：capturephoto.cpp
** 主 要 类：CapturePhoto
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   人脸图像采集界面
** 主要模块说明: 人脸图像采集过程中实时显示， 采集完成， 返回
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <unistd.h>

#include "capturephoto.h"
#include "ui_capturephoto.h"
#include "Common/common.h"

//*****************************
//*
//*类 名：QIdentifyCallback
//*作 用：回调函数 1，回调返回识别结果 2， 返回提示信息
//*
//*******************************/
//class FacePhotoEvent :public QObject
//{
//    Q_OBJECT
//public:

//    void onResult(QImage img)//返回识别结果
//    {
//        emit sigResult(img);
//    }

//signals:
//    void sigResult(QImage img);

//};

//FacePhotoEvent faceEvent;

///*****************************************************************************
//*                        人脸图像采集处理函数
//*  函 数 名：GetFacePhoto
//*  功    能：调用人脸采集图像接口，采集到图像后发射信号，供图像处理类使用
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：liuzhch
//*  创建时间：2018-10-20
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
void *GetFacePhoto(void* arg)
{
    //TO DO
    if(NULL == arg)
    {
        return (void *)0;
    }

    CapturePhoto* cp = static_cast<CapturePhoto *>(arg);

    VideoCapture cap;
    if(!cap.open(1))
    {
        cp->_stop=true;
        return -1;
    }
    Mat frame;

    cp->_stop = false;
    while (!cp->_stop)
    {
        cap >> frame;
        QImage im = cvMat2QImage(frame);
//        emit cp->sigUpdateImage(im);
        cp->slotGetFaceImage(im);
        usleep(30);
    }
    cap.release();


    return (void*)0;
}

CapturePhoto::CapturePhoto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CapturePhoto),
    _stop(true)
{
    ui->setupUi(this);

    //去除对话框标题栏，坐标重定位和填充背景
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, 0, 800, 600));
    //setBackgroundColor(QColor(242,241,240));


    //连接返回和保存信号和对应的槽
    connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(this, SIGNAL(sigUpdateImage(QImage)), this, SLOT(slotGetFaceImage(QImage)));
   // connect(&faceEvent,SIGNAL(sigResult(QImage)),this,SLOT(slotGetFaceImage(QImage)));
}

CapturePhoto::~CapturePhoto()
{
    delete ui;
}

bool CapturePhoto::setFileNameInfo(QString personInfo)
{
    persInfo = personInfo;

    return true;
}

bool CapturePhoto::initCamera()
{
    if(_stop){
        pthread_create(&thrID, NULL, GetFacePhoto, this);
    }
    return true;
}

void CapturePhoto::slotGetFaceImage(QImage img)
{
    if (!img.isNull()){
        ui->labFaceImageStream->setPixmap(QPixmap::fromImage(img, Qt::AutoColor));
    }
}

/*****************************************************************************
*                        返回操作处理函数
*  函 数 名：slotBack
*  功    能：槽函数，处理返回操作
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CapturePhoto::slotBack()
{
    //TO DO
    stopCapturing();
    close();
}

/*****************************************************************************
*                        保存采集到的当前人脸图像
*  函 数 名：slotSave
*  功    能：槽函数，存储当前采集的人脸图像
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CapturePhoto::slotSave()
{
    //TO DO
    stopCapturing();
    //关闭当前窗口
    close();
}


void CapturePhoto::stopCapturing()
{
    _stop= true;
    void *rtn_status;
    pthread_join(thrID, &rtn_status);
}
