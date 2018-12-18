/*****************************************************************************
** 文 件 名：FeatureManageForSave.cpp
** 主 要 类：
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
#include "FeatureManage.h"

const char * c_filename_iris_format="./IrisPath/EnrollImage/Eye/iris_%d_%d.jpg";
const char * c_filename_face_format="./FacePath/EnrollImage/Eye/iris_%d_%s.jpg";


/*****************************************************************************
*                         初始化参数
*  函 数 名：InitParam
*  功    能：初始化特征管理相关参数
*  说    明：由识别模块启动时调用
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-30
*  修 改 人：
*  修改时间：
*****************************************************************************/
void FeatureManage::InitParam()
{
    //创建相关路径
    QDir creatDir;
    QString strPath = APPIrisImagePath;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }
    strPath = APPFaceImagePath;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }
}

/*****************************************************************************
*                         载入特征
*  函 数 名：LoadTemplate
*  功    能：从相应文件夹中载入特征到内存
*  说    明：由识别模块启动时调用
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-30
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::vector<cv::Mat> FeatureManage::GetImage()
{
    std::vector<cv::Mat> vMats;

    return vMats;

}

/*****************************************************************************
*                         向特征内存中增加一个特征
*  函 数 名：AddTemplate
*  功    能：向特征内存中增加一个特征
*  说    明：由注册模块在保存注册结果时调用
*  参    数：feature：输入参数，待保存特征
*           name：输入参数，用户名称
*           featureFlag：输入参数，标记左眼还是右眼特征
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-30
*  修 改 人：
*  修改时间：
*****************************************************************************/
void FeatureManage::SaveImage(int uid,int serialnum,cv::Mat im)
{
    sprintf(_irisImagesName, c_filename_iris_format, uid, serialnum);

    //cun chu tu xiang
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
    compression_params.push_back(100); //在这个填入你要的图片质量

    cv::imwrite(_irisImagesName,im,compression_params);

}


/*****************************************************************************
*                         renlian向特征内存中增加一个特征
*  函 数 名：AddTemplate
*  功    能：向特征内存中增加一个特征
*  说    明：由注册模块在保存注册结果时调用
*  参    数：feature：输入参数，待保存特征
*           name：输入参数，用户名称
*           featureFlag：输入参数，标记左眼还是右眼特征
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-30
*  修 改 人：
*  修改时间：
*****************************************************************************/
void FeatureManage::SaveFaceImage(int uid,cv::Mat im,cv::Mat reg_face)
{
    sprintf(_faceImagesOriName, c_filename_face_format, uid, "ori");
    sprintf(_faceImagesRegName, c_filename_face_format, uid, "reg");

    //cun chu tu xiang
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
    compression_params.push_back(100); //在这个填入你要的图片质量

    cv::imwrite(_faceImagesOriName,im,compression_params);
    cv::imwrite(_faceImagesRegName,reg_face,compression_params);

}
