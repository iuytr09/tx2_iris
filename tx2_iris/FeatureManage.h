/*****************************************************************************
** 文 件 名：FeatureManageForDemo.h
** 主 要 类：CFeatureManageForDemo
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-12-30
**
** 修 改 人：
** 修改时间：
** 描  述:   临时文件，用于第一期演示时的特征管理
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef FEATUREMANAGE_H
#define FEATUREMANAGE_H
#include <QDir>
#include <QTextCodec>
#include<iostream>
#include<highgui/highgui.hpp>

//对注册特征个数及用户名长度进行设置
const int g_constMaxEnrFeatureNum = 100;
const int g_constUserNameLen = 20;


enum FeatureFlag
{
    FeatureLeft = 0,
    FeatureRight = 1
};

//设置路径
#define APPIrisImagePath ("./IrisPath/EnrollImage/Eye")
#define APPFaceImagePath ("./FacePath/EnrollImage/Eye")

//xie cheng dan li

//临时特征管理类
class FeatureManage
{
public:
    static QString s_strEnrImagePath;    //左眼注册图像保存路径

public:
    void InitParam();   //初始化参数
    std::vector<cv::Mat> GetImage();//载入特征
    void SaveImage(int uid, int serialnum, cv::Mat im);//向特征内存中增加一个特征
    void SaveFaceImage(int uid,cv::Mat im,cv::Mat reg_face);
private:
    char _irisImagesName[128];
    char _faceImagesOriName[128];
    char _faceImagesRegName[128];
};


#endif // FEATUREMANAGE_H
