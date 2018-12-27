/*****************************************************************************
** 文 件 名：interaction.h
** 主 要 类：Interaction
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-11-08
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
#ifndef INTERACTION_H
#define INTERACTION_H

#include <QMediaPlayer>
#include <sys/time.h>
#include <unistd.h>
#include <QPainter>

#include <thread>
#include <mutex>
#include <QTimer>

#include "Common/common.h"

const int g_constSoundInterval = 5;//发声间隔，只对注册中、识别中播放语音有效


class Interaction : public QObject
{
    Q_OBJECT
public:
    static Interaction* GetInteractionInstance();
    ~Interaction();
    void PlayInteractionSound(IN InteractionResultType resultType, IN IrisPositionFlag irisPositionFlag);        //播放人机交互语音提示，靠近、靠远等
    void ClearResult();
private:
    Interaction();
    Interaction(const Interaction&);//禁止拷贝赋值
    Interaction& operator=(const Interaction&);


private:
    static Interaction *s_pSingletonInteraction;
    static std::mutex s_interactionInstanceMutex;                   //获取Interaction实例mutex

    static  QMediaPlayer *s_sound;//播放语音的MediaObject
    static struct timeval s_tvSoundStart; //发声开始时间
    static bool s_updateTvStart;         //是否更新发声开始时间
    bool _resultFlag;               //结果标志位，表明目前是否有结果

    bool _infraredLEDOnFlag;             //红外灯是否开启标志
    QTimer *_timerForInfraredLED;       //红外灯点亮状态定时器

};

#endif // INTERACTION_H
