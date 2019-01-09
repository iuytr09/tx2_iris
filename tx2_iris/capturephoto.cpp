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
#include "Algorithm/face_algorith.h"




CapturePhoto::CapturePhoto(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CapturePhoto)//,
  //_stop(true)
{
    ui->setupUi(this);

    //去除对话框标题栏，坐标重定位和填充背景
    setWindowFlags(Qt::FramelessWindowHint);
  //  setGeometry(QRect(0, 0, 800, 600));
    //setBackgroundColor(QColor(242,241,240));


    _pUsbVideoCap = UsbVideoCap::GetInstance();
    if(connect(_pUsbVideoCap,SIGNAL(OnUpdateImage(cv::Mat)),this,SLOT(slotImageUpdate(cv::Mat))))
    {
        std::cout<<"guanlian chenggong!"<<std::endl;
    }else{
        std::cout<<"guanlian shibai!"<<std::endl;
    }

    //连接返回和保存信号和对应的槽
    connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(this, SIGNAL(sigUpdateImage(QImage)), this, SLOT(slotGetFaceImage(QImage)));

}

CapturePhoto::~CapturePhoto()
{
    Face_Algorith::GetInstance()->SetNoWork();
    disconnect(_pUsbVideoCap,SIGNAL(OnUpdateImage(cv::Mat)),this,SLOT(slotImageUpdate(cv::Mat)));
    delete ui;
}

bool CapturePhoto::setFileNameInfo(QString personInfo)
{
    Face_Algorith::GetInstance()->SetEnrollWork();
    persInfo = personInfo;
    return true;
}


void CapturePhoto::slotImageUpdate(cv::Mat im)
{
    QImage img = cvMat2QImage(im);
    if (!img.isNull()){
        QPixmap pixmap = QPixmap::fromImage(img);
        pixmap.scaled(ui->labFaceImageStream->size(), Qt::KeepAspectRatio);
        ui->labFaceImageStream->setScaledContents(true);
        ui->labFaceImageStream->setPixmap(pixmap);
       // ui->labFaceImageStream->setPixmap(QPixmap::fromImage(img, Qt::AutoColor));
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
    //关闭当前窗口
    close();
}

