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
    setGeometry(QRect(0, 0, 800, 600));

    qRegisterMetaType<LRSucFailFlag>("LRSucFailFlag");
    qRegisterMetaType<IrisPositionFlag>("IrisPositionFlag");

    _dialogEnroll = new DialogEnroll(this);
    _dialogEnroll->hide();

    _pirisA=IRIS_Algorith::GetInstance();

    connect(_pirisA,SIGNAL(sigEnrollSuccess(int)),this,SLOT(slotEnrollSuccess(int)));

    _frameNum = 0;
    _captureImageTimerId = this->startTimer(1000);
}


 void Enroll::slotEnrollSuccess(int state)
 {
     if(state==0){
          ui->labtipMsg->setText(QString("%1 zhu ce cheng gong!").arg(_personInfo.name));
          ui->btnSaveEnrollResult->setEnabled(true);
     }else{
          ui->labtipMsg->setText(QString("%1 zhu ce shi bai!").arg(_personInfo.name));
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
   // _capImgThread.join();
     disconnect(_pirisA,SIGNAL(sigEnrollSuccess(int)),this,SLOT(slotEnrollSuccess(int)));
 //   delete _enrollAlgApi;
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

    on_rbtnBothEye_clicked();//默认为双眼模式注册

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


/*****************************************************************************
*                         注册模块采图线程
*  函 数 名：CapImgThread
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* Enroll::CapImgThread(void* pParam)
{
    Enroll *pCGDlg = static_cast <Enroll*> (pParam);
  //  int funResult = E_ALGAPI_ERROR_BASE;

    while(pCGDlg->_capImgFlag)
    {
        if(pCGDlg->_signaledFlag)
        {
            std::lock_guard<std::mutex> lck (pCGDlg->_capImgMutex, std::adopt_lock);
//            funResult = pCGDlg->_enrollAlgApi->SyncCapIrisImg(pCGDlg->_grayCapImgBuffer + g_constIrisPPMHeaderLength, pCGDlg->_lrIrisClearFlag, pCGDlg->_irisPositionFlag);
//            funResult = pCGDlg->_enrollAlgApi->SyncCapIrisImg(g_infraredDistanceValue, pCGDlg->_grayCapImgBuffer, s_lrIrisClearFlag, s_irisPositionFlag);
            //funResult = pCGDlg->_enrollAlgApi->SyncCapIrisImg(pCGDlg->s_enrollInteraction->s_infraredDistanceValue, pCGDlg->_grayCapImgBuffer, s_lrIrisClearFlag, s_irisPositionFlag);
      //      if(E_ALGAPI_OK == funResult)
       //     {
       //         imPaintEvent.RaisePaintCapImgEvent();

                //^^^^^^^^^^^^^^^
//                pCGDlg->_frameNum++;
                //vvvvvvvvvvvvvvv
       //     }
        }
        else
        {
//        usleep(20);
//            sleep(0);
        }
    }
    return (void*)0;
}

/*****************************************************************************
*                         异步注册回调函数
*  函 数 名：
*  功    能：异步注册回调函数
*  说    明：在回调函数中取异步注册结果
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-26
*  修 改 人：
*  修改时间：
//*****************************************************************************/
//int Enroll::CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag)
//{
//    QImage img;//处理注册结果时目前只是为了适应ProcessResult的接口参数，实际没有使用

//    if(E_ALGAPI_OK == funResult)
//    {
//        //注册成功，处理注册结果
//        s_enrIrisL = enrIrisL;
//        s_enrIrisR = enrIrisR;
//        s_enrLrFlag = lrFlag;

////        s_enrollInteraction->ProcessResult(EnrollSuccess, img);
//        s_enrollInteraction->PlayInteraction(EnrollSuccess, s_lrIrisClearFlag, s_irisPositionFlag, img);

//    }
//    else if(E_ALGAPI_ENR_IDEN_FAILED == funResult)
//    {
//        //注册失败，在界面显示注册失败
//        //TO DO
////        s_enrollInteraction->ProcessResult(EnrollFailed, img);
//        s_enrollInteraction->PlayInteraction(EnrollFailed, s_lrIrisClearFlag, s_irisPositionFlag, img);

//    }
//    else
//    {
//        //其他情况，如注册过程中注册被人为停止，不再处理，直接返回即可

//    }

//    s_enrollInteraction->ClearResult();

//    imPaintEvent.RaisePaintEnrollResultEvent(funResult, enrIrisL, numEnrL, enrIrisR, numEnrR, lrFlag);
//    return 0;
//}

/*****************************************************************************
*                         设置注册模式为双眼模式
*  函 数 名：on_rbtnBothEye_clicked
*  功    能：设置注册模式为双眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnBothEye_clicked()
{
//    int funResult = E_ALGAPI_DEFAUL_ERROR;
//   // funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
//    if(E_ALGAPI_OK == funResult)
//    {
//        _enrollParamStruct.EyeMode = BothEye;
//  //      funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
//    }

    return;
}

/*****************************************************************************
*                         设置注册模式为左眼模式
*  函 数 名：on_rbtnLeftEye_clicked
*  功    能：设置注册模式为左眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnLeftEye_clicked()
{
//    int funResult = E_ALGAPI_DEFAUL_ERROR;
// //   funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
//    if(E_ALGAPI_OK == funResult)
//    {
//        _enrollParamStruct.EyeMode = LeftEye;
//   //     funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
//    }
    return;
}

/*****************************************************************************
*                         设置注册模式为右眼模式
*  函 数 名：on_rbtnRightEye_clicked
*  功    能：设置注册模式为右眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnRightEye_clicked()
{
//    int funResult = E_ALGAPI_DEFAUL_ERROR;
//    //funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
//    if(E_ALGAPI_OK == funResult)
//    {
//        _enrollParamStruct.EyeMode = RightEye;
//    //    funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
//    }
    return;
}

/*****************************************************************************
*                         设置注册模式为任意眼模式
*  函 数 名：on_rbtnAnyEye_clicked
*  功    能：设置注册模式为任意眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnAnyEye_clicked()
{
//    int funResult = E_ALGAPI_DEFAUL_ERROR;
// //   funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
//    if(E_ALGAPI_OK == funResult)
//    {
//        _enrollParamStruct.EyeMode = AnyEye;
////        funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
//    }
    return;
}

/*****************************************************************************
*                         点击注册结果左眼第一幅小图浏览大图
*  函 数 名：on_btnIrisImgLeft1_clicked
*  功    能：点击注册结果左眼第一幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgLeft1_clicked()
{
   // _dialogEnroll->XferIrisInfo(&s_enrIrisL[0]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果左眼第二幅小图浏览大图
*  函 数 名：on_btnIrisImgLeft2_clicked
*  功    能：点击注册结果左眼第二幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgLeft2_clicked()
{
//    _dialogEnroll->XferIrisInfo(&s_enrIrisL[1]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果左眼第三幅小图浏览大图
*  函 数 名：on_btnIrisImgLeft3_clicked
*  功    能：点击注册结果左眼第三幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgLeft3_clicked()
{
  //  _dialogEnroll->XferIrisInfo(&s_enrIrisL[2]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果右眼第一幅小图浏览大图
*  函 数 名：on_btnIrisImgRight1_clicked
*  功    能：点击注册结果右眼第一幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgRight1_clicked()
{
  //  _dialogEnroll->XferIrisInfo(&s_enrIrisR[0]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果右眼第二幅小图浏览大图
*  函 数 名：on_btnIrisImgRight2_clicked
*  功    能：点击注册结果右眼第二幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgRight2_clicked()
{
   // _dialogEnroll->XferIrisInfo(&s_enrIrisR[1]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果右眼第三幅小图浏览大图
*  函 数 名：on_btnIrisImgRight3_clicked
*  功    能：点击注册结果右眼第三幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgRight3_clicked()
{
  //  _dialogEnroll->XferIrisInfo(&s_enrIrisR[2]);
    _dialogEnroll->exec();
}

//^^^^^^^^^^^^^^^^^^^^^^^^
void Enroll::timerEvent(QTimerEvent *event)
{
    QString strFrameNum;
    if (_captureImageTimerId == event->timerId())
    {
        strFrameNum.setNum(_frameNum);
       // ui->lineEditDepartment->setText(strFrameNum);
        _frameNum = 0;
    }

}
//vvvvvvvvvvvvvvvvvvvvvvvvvv

