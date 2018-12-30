/*****************************************************************************
** 文 件 名：Identity.h
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
#ifndef IDENTITY_H
#define IDENTITY_H

#include <QtWidgets/QWidget>
//#include "../USBAPI/simplelock.h"
#include <QVector>

#include <mutex>
#include <thread>
#include <condition_variable>
#include "interaction.h"
#include "Algorithm/face_algorith.h"
#include "Algorithm/iris_algorith.h"
#include "VideoCapture/usbvideocap.h"


namespace Ui {
class DialogIdentify;
}
enum IdentResultState
{
    SuccessState=0,
    FailedState,
    ContinueState ,
    KeepState,
    UnKnownState
};

class Person;
class DialogIdentify : public QWidget
{
    Q_OBJECT
    
public:
    explicit DialogIdentify(QWidget *parent = 0);
    ~DialogIdentify();

signals:

   void sigGoBack();              //返回考勤识别
   void sigLoginSucceed(int state); //信号，登陆是否成功

public slots:

    void timerEvent(QTimerEvent *event);//界面时间更新
    void slotImageUpdate(cv::Mat im);     //界面图像更新
    void updateIdentState(int, std::vector<std::vector<float>>);
    void IrisImageUpdate(QImage im); //hong mo
    void StopIdent();       //停止识别

    void StartAttendIdent();       //开始考勤识别
    void StartLoginIdent();        //开始管理员登陆识别

    void IdentIrisResult(PersonInfo p);         //考勤识别结果
    void LoginIdentResult(long personIndex, int flag);                      //管理员登陆识别结果
    void IdentFaceResult(PersonInfo info);         //考勤识别结果

    void  slotFaceState(InteractionResultType,IrisPositionFlag);
    void slotBoxsChanged(std::vector<st_EYE_LOC_INFO>  box);
private:

    Ui::DialogIdentify *ui;

    //退出线程
    bool  _exitIdentThread;


    QVector<QString>  _personName;             //考勤人员信息
    QVector<QString>  _loginPersonName;        //管理员人员信息

    std::thread _identCapImgThread;            //识别模块图像显示线程
    std::mutex _identCapImgMutex;              //识别模块图像采集mutex
    std::condition_variable _identCapImgCV;    //识别模块图像采集condition_variable
    bool _identCapImgFlag;                     //识别模块图像采集flag

    int _count;                                //帧率
    bool _isClose;                             //是否停止识别
    bool  _isLoginIdent;                      //是否开始识别

    UsbVideoCap* _pUsbVideoCap;
    Face_Algorith* _pFaceAlgorith;

    //人机交互类
 //   static Interaction *_identInteraction;
    IdentResultState  _identResultState;           //识别结果
    QTimer *_timerStartIdent;                      //识别成功或失败时显示框显示1s钟

    QImage  _top_im;   //hui zhi xian kuang

};

#endif // IDENTITY_H
