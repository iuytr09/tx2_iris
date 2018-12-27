/*****************************************************************************
** 文 件 名：interaction.cpp
** 主 要 类：Interaction
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-08
**
** 修 改 人：
** 修改时间：
** 描  述:   人机交互，负责语音提示、界面提示等
** 主要模块说明: 人机交互类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "interaction.h"
#include <math.h>   //为了在distanceCB中使用pow

#include <QDir>



////播放语音的MediaObject
QMediaPlayer* Interaction:: s_sound = nullptr;


struct timeval Interaction::s_tvSoundStart;//发声开始时间
bool Interaction::s_updateTvStart = true;//是否更新发声开始时间，初始化为true


std::mutex Interaction::s_interactionInstanceMutex;
Interaction* Interaction::s_pSingletonInteraction = NULL;

/*****************************************************************************
*                         获取Interaction实例
*  函 数 名：GetInteractionInstance
*  功    能：获取CAPIBase实例
*  说    明：Interaction采用单件模式
*  参    数：NULL
*
*  返 回 值：!NULL: Interaction实例指针
*			NULL：获取失败
*  创 建 人：liuzhch
*  创建时间：2014-02-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
Interaction* Interaction::GetInteractionInstance()
{
    // 双重检查，提高性能
    if (NULL == s_pSingletonInteraction)
    {
        std::lock_guard<std::mutex> lck (s_interactionInstanceMutex);

        if (NULL == s_pSingletonInteraction)
        {
            s_pSingletonInteraction = new Interaction();
        }
    }

    return s_pSingletonInteraction;
}


Interaction::Interaction()
{
    _resultFlag = false;//初始化时没有结果
    s_sound = new QMediaPlayer();
    s_sound->setMedia(QUrl::fromLocalFile("/home/nvidia/work/build-tx2_iris-Desktop-Debu/aaa.wav"));
}

Interaction::~Interaction()
{
}
/*****************************************************************************
*                         清除结果
*  函 数 名：ClearResult
*  功    能：清除结果
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-10-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::ClearResult()
{
    _resultFlag = false;
}

/*****************************************************************************
*                         播放人机交互语音提示，靠近、靠远等
*  函 数 名：PlayInteractionSound
*  功    能：播放人机交互语音提示，靠近、靠远等
*  说    明：
*  参    数：resultType：输入参数，注册、识别的结果类型
*           irisPositionFlag：输入参数，位置信息标志
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-10-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Interaction::PlayInteractionSound(IN InteractionResultType resultType, IN IrisPositionFlag irisPositionFlag)
{

    s_sound->play();


////    qDebug()<<QDir::currentPath();

//    struct timeval tvSoundEnd;
//    int durationTime = 0;//声音间隔

//    if(s_updateTvStart)
//    {
//        //更新声音播放开始时间
//        gettimeofday(&s_tvSoundStart,NULL);
//        s_updateTvStart = false;
//    }

//    //获取当前时间
//    gettimeofday(&tvSoundEnd,NULL);
//    durationTime = tvSoundEnd.tv_sec - s_tvSoundStart.tv_sec;

//    if( (durationTime < g_constSoundInterval) && (ResultUnknown == resultType) )
//    {
//        //当前时间与声音播放开始时间间隔太小，并且没有注册或识别结果，则不播放声音
//        return;
//    }
//    else
//    {
//        //达到间隔时间，播放声音，将更新声音播放时间标志设为true
//        s_updateTvStart = true;
//    }

//    if(ResultUnknown == resultType)
//    {
//        switch(irisPositionFlag)
//        {
//        case Far:
//            s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/010closer.wav"));
//            break;

//        case Near:
//           s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/018farther.wav"));
//            break;

//        case Left:
//            s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/104right.wav"));
//            break;

//        case Right:
//            s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/105left.wav"));
//            break;

//        case Up:
//               s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/102down.wav"));
//            break;

//        case Down:
//              s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/103up.wav"));
//            break;

//        case OK:
//           s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/100keep.wav"));
//            break;

//        default:
//            break;
//        }
//    }
//    else
//    {
//        if((s_sound) && (IdenKeepLastStatus != resultType))
//        {
//            s_sound->stop();
//        }

//        switch(resultType)
//        {
//        case EnrollSuccess:
//            s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/003enrollsucc.wav"));
//            break;
//        case EnrollFailed:
//            s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/006enrollstop.wav"));
//            break;
//        case IdenSuccess:
//            s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/002idensucc.wav"));
//            break;
//        case IdenFailed:
//            s_sound->setMedia(QUrl::fromLocalFile("./sounds/chs/005retry.wav"));
//            break;
//        default:
//            break;
//        }

//    }

//    if((s_sound) && (false == _resultFlag) && (IdenKeepLastStatus != resultType))
//    {
//        s_sound->play();
//    }

}
