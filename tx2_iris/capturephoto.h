/*****************************************************************************
** 文 件 名：capturephoto.h
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
#pragma once
#include <QtWidgets/QDialog>
#include <QImage>
#include <pthread.h>


void *GetFacePhoto(void* arg);


namespace Ui {
class CapturePhoto;
}

class CapturePhoto : public QDialog
{
//    friend void *GetFacePhoto(void* arg);

    Q_OBJECT    
public:
    explicit CapturePhoto(QWidget *parent = 0);
    ~CapturePhoto();
    //设置人脸图像文件名信息
    bool setFileNameInfo(QString personInfo);

//    bool initCamera();
     void SetFaceImage(QImage);
signals:
//    void sigUpdateImage(QImage);


protected slots:

    void slotBack();
    void slotSave();

protected:
//    void stopCapturing();

private:
    Ui::CapturePhoto *ui;
//    pthread_t thrID;
//    bool _stop;
    QString persInfo;
};
