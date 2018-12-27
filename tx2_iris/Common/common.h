/******************************************************************************************
** 文件名:   common.h
×× 主要类:   (无)
**  
** Copyright (c) shanggushijue
** 创建人:   liuzc
** 日  期:   2018-10-06
** 修改人:   
** 日  期:
** 描  述:   公共头文件定义
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
******************************************************************************************/
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <QObject>
#include<QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;


//参数输入输出指示宏定义
#define IN
#define OUT

//常量定义
const int SEX_MALE = 0;     //性别： 男
const int SEX_FAMALE = 1;   //性别： 女


const int PERSON_USER = 1;   //用户类型： 普通用户
const int PERSON_ADMIN = 2;    //用户类型： 管理员


const int IMAGE_WIDTH = 480;
const int IMAGE_HEIGHT = 640;
const int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;




//错误码定义
const int E_OK = 0;       //操作成功

//人机交互结果类型
enum InteractionResultType
{
    IrisEnrollSuccess,  //虹膜注册成功
    IrisEnrollFailed,   //虹膜注册失败
    IrisEnrollGlassesExist,   //虹膜注册存在带眼镜
    IrisEnrollSangleEye,   //虹膜注册存在单只眼睛
//    IrisEnrollQuality,   //虹膜注册质量差
    IrisEnrollPerformig,//虹膜注册中 位置信息有效
    IrisIdenSuccess,    //虹膜识别成功
    IrisIdenFailed,     //虹膜识别失败
    IrisIdenKeepLastStatus,//保存识别上一次结果状态，只界面显示，不播放语音
    IrisIdenPerforming, //虹膜识别中
    FaceEnrollSuccess,  //人脸注册成功
    FaceEnrollFailed,   //人脸注册失败
    FaceEnrollPerformig,//人脸注册中  位置信息有效
    FaceIdenSuccess,    //人脸识别成功
    FaceIdenFailed,     //人脸识别失败
    FaceIdenforming, //人脸识别中
    ResultUnknown   //结果未知或者目前没有结果

};

//位置信息标志
enum IrisPositionFlag
{
    Far				=	0,	//位置偏远
    Near			=	1,  //位置偏近
    Left			=	2,  //位置偏左
    Right			=	3,  //位置偏右
    Up				=	4,  //位置偏上
    Down			=	5,  //位置偏下
    OK				=	6,  //位置合适
    POSEWRONG       =   7,  //位置不正
    Quality         =   8,  //质量不合格
    Unknown         =   9   //位置未知，不用提示
};

//注册图像个数
const int g_constMaxEnrollImNum =	3;		//注册过程中算法允许的最大注册图像数

//设备 算法逻辑控制的输入参数
//参数1. eyeMode——采图、注册、识别时的眼睛模式
enum APIEyeMode
{
    BothEye         =   0,      //双眼
    LeftEye         =   1,      //左眼
    RightEye        =   2,      //右眼
    AnyEye          =   3       //任意眼，注册模式下，任意眼无效
};



//错误宏函数
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)


//注册与人员管理模块的信息交互
class PersonInfo
{
public:
    //TO DO
    PersonInfo();
    ~PersonInfo();

public:
    //TO DO
    int  id;
    QString name;
    QString depart_name;
};


//注册结果质量分数最小分数
const double MinEnrollQualityScore = 0.8;



//*****************************
//*
//*类 名：QIdentifyCallback
//*作 用：回调函数 1，回调返回识别结果 2， 返回提示信息
//*
//*******************************/
class ReDrawEvent :public QObject
{
    Q_OBJECT
public:
//    ReDrawEvent(){}
//    ~ReDrawEvent(){}
    void onResult()//返回识别结果
    {
        emit sigResult();
    }
    void onMsg(QString str)//返回提示信息
    {
        emit sigMsg(str);
    }
    //识别界面重绘
    void OnDrawIdent(cv::Mat im)
    {
        emit sigDrawIdent(im);
    }

signals:
    void sigDrawIdent(cv::Mat im);
    void sigResult();
    void sigMsg(QString str);
};

//*****************************
//*
//*类 名：QIdentifyCallback
//*作 用：回调函数 1，回调返回识别结果 2， 返回提示信息
//*
//*******************************/
class IdentEvent :public QObject
{
    Q_OBJECT
public:

    void onResult(long index,int flag)//返回识别结果
    {
        emit sigResult(index,flag);
    }
    void onLoginResult(long index,int flag)//返回管理员识别结果
    {
        emit sigLoginResult(index,flag);
    }
    void onMsg(QString str)//返回提示信息
    {
        emit sigMsg(str);
    }
    //识别界面重绘
    void OnDrawIdent()
    {
        emit sigDrawIdent();
    }

signals:
    void sigDrawIdent();
    void sigResult(long index,int flag);
    void sigLoginResult(long index,int flag);
    void sigMsg(QString str);
};

/*****************************************************************************
*                         串口获取红外测距值的回调函数
*  函 数 名：cvMat2QImage
*  功    能:mat->QImage
*  说    明：
*  参    数：mat
*
*  返 回 值：QImage
*
*  创 建 人：liuzhch
*  创建时间：2018-10-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
QImage static cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        std::cout<< "CV_8UC4"<<std::endl;
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        std::cout<< "ERROR: Mat could not be converted to QImage."<<std::endl;
        return QImage();
    }
}

/*****************************************************************************
*                         串口获取红外测距值的回调函数
*  函 数 名：QImage2cvMat
*  功    能：QIMAGE->Mat
*  说    明：
*  参    数：image：输入tuxiang

*
*  返 回 值：Mat
*
*  创 建 人：liuzhch
*  创建时间：2018-10-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
cv::Mat static QImage2cvMat(QImage image)
{
    cv::Mat mat;
   std::cout << image.format()<<std::endl;
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}
