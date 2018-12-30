/*****************************************************************************
** 文 件 名：enroll.cpp
** 主 要 类：Enroll
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   注册模块
** 主要模块说明: 注册类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "enroll.h"
#include "ui_enroll.h"
#include "dialogenroll.h"
#include <sys/time.h>
#include "Algorithm/iris_algorith.h"
//#include <QDebug>
#include <QDir>
//--------------------------画图事件响应-----------------------------
ImagePaintEvent imPaintEvent;

/*****************************************************************************
*                         注册模块发射绘制采集到的图像信号
*  函 数 名：RaisePaintCapImgEvent
*  功    能：采图
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void ImagePaintEvent::RaisePaintCapImgEvent()
{
    emit SigPaintCapImg();
}


//--------------------------画图事件响应-----------------------------

/*****************************************************************************
*                         Enroll构造函数
*  函 数 名：Enroll
*  功    能：分配Enroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
Enroll::Enroll(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Enroll)
{
    ui->setupUi(this);


    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
    //setGeometry(QRect(0, 0, 800, 600));

    qRegisterMetaType<IrisPositionFlag>("IrisPositionFlag");

    _dialogEnroll = new DialogEnroll(this);
    _dialogEnroll->hide();

    _pirisA=IRIS_Algorith::GetInstance();

    connect(_pirisA,SIGNAL(sigIrisState(InteractionResultType,IrisPositionFlag)),this,SLOT(slotEnrollSuccess(InteractionResultType,IrisPositionFlag)));

    _frameNum = 0;
    _captureImageTimerId = this->startTimer(1000);
}


 void Enroll::slotEnrollSuccess(InteractionResultType type,IrisPositionFlag flag)
 {
     if(type==InteractionResultType::IrisEnrollSuccess){
          ui->labtipMsg->setText(QString("%1 zhu ce cheng gong!").arg(_personInfo.name));
          ui->btnSaveEnrollResult->setEnabled(true);
     }else if(type==InteractionResultType::IrisEnrollFailed){
          ui->labtipMsg->setText(QString("%1 zhu ce shi bai!").arg(_personInfo.name));
     }else{
          ui->labtipMsg->setText(QString("%1 zhu ce zhong...!").arg(_personInfo.name));
     }


 }

/*****************************************************************************
*                         Enroll析构函数
*  函 数 名：~Enroll
*  功    能：释放Enroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
Enroll::~Enroll()
{
    _capImgFlag = false;
     disconnect(_pirisA,SIGNAL(sigIrisState(InteractionResultType,IrisPositionFlag)),this,SLOT(slotEnrollSuccess(InteractionResultType,IrisPositionFlag)));
    delete ui;
}

/*****************************************************************************
*                         设置人员信息，并对界面作相应初始化，由人员管理模块调用
*  函 数 名：SetPersonInfo
*  功    能：设置人员信息，并对界面作相应初始化，由人员管理模块调用
*  说    明：
*  参    数：personInfomation：人员信息
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-31
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::SetPersonInfo(PersonInfo *personInfomation)
{
    //设置界面
    ui->widgetDisplayIm->show();
    ui->widgetEnrollResultIms->hide();
    ui->btnSaveEnrollResult->setEnabled(false);
    ui->btnEnroll->setText(QObject::tr("开始注册"));
    ui->btnEnroll->setIcon(QIcon(":/image/开始.png"));
    ui->btnEnroll->setIconSize(QSize(40,40));


    _personInfo = *personInfomation;

//    CFeatureManageForDemo enrFeatureManage;
//    enrFeatureManage.s_strUserName = QString::fromLocal8Bit(_personInfo.name.c_str());
//    QString qstrDepartment = QString::fromLocal8Bit(_personInfo.depart.c_str());

    ui->lineEditUserName->setText(_personInfo.name);
    ui->lineEditDepartment->setText(_personInfo.depart_name);


    _signaledFlag = true;//采图线程开始采图
    _btnEnrollClicked = false;//启动时注册按钮没有被点击

    //on_rbtnBothEye_clicked();//默认为双眼模式注册

    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //_captureImageTimerId = this->startTimer(1000);
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
}

/*****************************************************************************
*                         开始注册
*  函 数 名：on_btnEnroll_clicked
*  功    能：开始注册
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnEnroll_clicked()
{
    ui->widgetEnrollResultIms->hide();
    ui->widgetDisplayIm->show();

    ui->btnSaveEnrollResult->setEnabled(false);

    if(_btnEnrollClicked)
    {
        //如果注册按钮被点击过，则停止注册
        //_enrollAlgApi->StopEnroll();
        ui->btnEnroll->setText(QObject::tr("开始注册"));
        ui->btnEnroll->setIcon(QIcon(":/image/开始.png"));
        ui->btnEnroll->setIconSize(QSize(40,40));
        _btnEnrollClicked = false;
        _pirisA->stop_Enroll();

    }
    else
    {
        //如果注册按钮没有被点击过，则开始注册
        ui->btnEnroll->setText(QObject::tr("停止注册"));
        ui->btnEnroll->setIcon(QIcon(":/image/停止.png"));
        ui->btnEnroll->setIconSize(QSize(40,40));
        _btnEnrollClicked = true;

        _pirisA->start_Enroll();
    }

}

/*****************************************************************************
*                         注册模块返回，不保存结果
*  函 数 名：on_btnEnrollReturn_clicked
*  功    能：注册模块返回，不保存结果
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnEnrollReturn_clicked()
{
    //不保存注册虹膜图像并退出
    //_enrollAlgApi->StopEnroll();
    _signaledFlag = false;//注册界面关闭，不再采图
    close();
    emit SigEnrollResult(false);
}

/*****************************************************************************
*                         保存结果，注册模块返回
*  函 数 名：on_btnSaveEnrollResult_clicked
*  功    能：保存结果，注册模块返回
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnSaveEnrollResult_clicked()
{
      emit SigEnrollResult(true);
}

/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：CapImageUpdate
*  功    能：注册模块将采集到的图像显示到界面
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::CapImageUpdate(QImage im)
{
    if(!im.isNull())
    {
        ui->labFace->setPixmap(QPixmap::fromImage(im));
        //_frameNum++;
    }
}


/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：CapImageUpdate
*  功    能：注册模块将采集到的图像显示到界面
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::IrisImageUpdate(QImage im)
{
    if(!im.isNull())
    {
        QPixmap pixmap = QPixmap::fromImage(im);
        pixmap.scaled(ui->labRealTimeIm->size(), Qt::KeepAspectRatio);
        ui->labRealTimeIm->setScaledContents(true);
        ui->labRealTimeIm->setPixmap(pixmap);
        _frameNum++;
    }
}


void Enroll::timerEvent(QTimerEvent *event)
{
    QString strFrameNum;
    if (_captureImageTimerId == event->timerId())
    {
        strFrameNum.setNum(_frameNum);
        _frameNum = 0;
    }

}

