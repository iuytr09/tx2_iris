
/*****************************************************************************
** 文 件 名：Identity.cpp
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
#include <unistd.h>
#include <iostream>
#include "dialogidentify.h"
#include "ui_dialogidentify.h"
#include "time.h"
#include "QDateTime"
#include "QTime"
#include <QtCore>
#include "QThread"
#include <sys/time.h>
#include <QMessageBox>
#include<QPainter>
#include<QRect>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;




DialogIdentify::DialogIdentify(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DialogIdentify)
{
    ui->setupUi(this);
    /**********界面时间实时显示*****************/
    //获取系统现在的时间
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");//设置系统时间显示格式
    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime);


    _top_im=QImage(480,640,QImage::Format_ARGB32);
    _top_im.fill(0);
    //设置界面背景
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/背景.jpg")));
    ui->widgetInfo->setPalette(palette);

    _isClose = false;
    _isLoginIdent = false;
    _identResultState = ContinueState;

    //新建定时器
    _timerStartIdent = new QTimer();

    _pFaceAlgorith = Face_Algorith::GetInstance();
    _pFaceAlgorith->StartWorker();

    connect(_pFaceAlgorith,SIGNAL(sigIdentSucsses(PersonInfo)),this,SLOT(IdentFaceResult(PersonInfo)));
    //    connect(_pFaceAlgorith,SIGNAL(sigIdentState(int, std::vector<std::vector<float>>)),this,SLOT(updateIdentState(int, std::vector<std::vector<float>>)));
    connect(_pFaceAlgorith,SIGNAL(sigFaceState(InteractionResultType,IrisPositionFlag)),this,SLOT(slotFaceState(InteractionResultType,IrisPositionFlag)));
    _pUsbVideoCap = UsbVideoCap::GetInstance();


    if(connect(_pUsbVideoCap,SIGNAL(OnUpdateImage(cv::Mat)),this,SLOT(slotImageUpdate(cv::Mat))))
    {
        std::cout<<"guan lian chenggong!"<<std::endl;
    }else{
        std::cout<<"guan lian shibai!"<<std::endl;
    }
}

DialogIdentify::~DialogIdentify()
{
    _exitIdentThread = false;
    disconnect(_pUsbVideoCap,SIGNAL(OnUpdateImage(cv::Mat)),this,SLOT(slotImageUpdate(cv::Mat)));
    delete _timerStartIdent;
    delete ui;
}

void DialogIdentify::IdentFaceResult(PersonInfo info){
    ui->labStatu->setText(info.name + " 人脸识别成功!");
}

/*****************************************************************************
*                        函数
*  函 数 名：updateIdentState
*  功    能： 根据状态改变画笔的颜色 绘制动态框
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::updateIdentState(int state, std::vector<std::vector<float>> face_boxs){
    _top_im.fill(0);

    QPainter painter(&_top_im);
    //画想要画的东西
    if(state==0){
        painter.setPen(QPen(Qt::green,5,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));

    }else{
        painter.setPen(QPen(Qt::red,5,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));
    }
    painter.setBrush(Qt::NoBrush);
    //more boxs
    for(int i=0;i<face_boxs.size();i++){
        if(face_boxs.at(i).size()>2){
            float lt_x=face_boxs.at(i)[0],lt_y=face_boxs.at(i)[1],rb_x=face_boxs.at(i)[0],rb_y=face_boxs.at(i)[1];
            for (int j=2;j<face_boxs.at(i).size()-1;j+=2){

                if(face_boxs.at(i).at(j)>rb_x){
                    rb_x=face_boxs.at(i).at(j);
                }
                if(face_boxs.at(i).at(j)<lt_x){
                    lt_x =face_boxs.at(i).at(j);
                }

                if(face_boxs.at(i).at(j+1)>rb_y){
                    rb_y=face_boxs.at(i).at(j+1);
                }
                if(face_boxs.at(i).at(j+1)<lt_y){
                    lt_y =face_boxs.at(i).at(j+1);
                }

            }
            QRect rect(int(lt_x),int(lt_y),int(rb_x),int(rb_y));//构造一个矩形
            std::cout<<int(lt_x)<<" "<<int(lt_y)<<" "<<int(rb_x)<<" "<<int(rb_y)<<std::endl;
            painter.drawRect(int(lt_x),int(lt_y),int(rb_x)-int(lt_x),int(rb_y)-int(lt_y));
            // painter.drawRect(rect);
        }
    }
    painter.end();

}

/*****************************************************************************
*                        函数
*  函 数 名：updateIdentState
*  功    能： 根据状态改变画笔的颜色 绘制动态框
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::slotFaceState(InteractionResultType ret,IrisPositionFlag flag){
    std::cout<<"InteractionResultType:"<<ret<<", IrisPositionFlag:"<<flag<<std::endl;

    //1\ kong zhi hong wei deng
    //2\ kongzhi hong mo shi bie

}
/*****************************************************************************
*                        开始识别
*  函 数 名： StartIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StartAttendIdent()
{

    //ren lian shi bie
    _pFaceAlgorith->SetIdentifyWork();


    _identResultState = UnKnownState;
    _timerStartIdent->stop();

    ui->labIsLogin->setText(QString::fromUtf8("考勤识别"));

    _isLoginIdent = false;

    if(error!=0)
    {
        std::cout<<"开始识别失败！"<<std::endl;
        // QMessageBox::information(this, QString::fromUtf8("识别"), QString::fromUtf8("识别失败！"));
    }
    else
    {
        _isClose = false;
    }
}


/*****************************************************************************
*                        管理员登陆开始识别
*  函 数 名： StaSLOon_start_work_action_triggeredTrtIdent
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：     connect(_pIrisVideo,SIGNAL(sigFramed(cv::Mat)),this,SLOT(iris_image(cv::Mat)));
*****************************************************************************/
void DialogIdentify::StartLoginIdent()
{
    ui->labIsLogin->setText(QString::fromUtf8("管理员登陆识别"));
    _isClose = false;
    _isLoginIdent =true;
}

/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：IdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::IdentResult(int flag)
{
    if(flag==-1)
    {
        ui->labStatu->setText(QString::fromUtf8("识别失败"));
        _identResultState = FailedState;
    }
    else if(flag == 0)
    {

        ui->labStatu->setText(QString::fromUtf8("识别成功！"));
        _identResultState = SuccessState;
    }
    else
    {
        _identResultState = ContinueState;

    }
}


/*****************************************************************************
*                        显示识别结果函数
*  函 数 名：LoginIdentResult
*  功    能：识别结果
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::LoginIdentResult(long personIndex,int flag)
{
    if(personIndex<0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别失败"));
    }
    else if(personIndex == 0)
    {
        ui->labStatu->setText(QString::fromUtf8("识别中..."));
    }
    else
    {
        if(personIndex<_personName.size())
        {
            ui->labStatu->setText(_loginPersonName[personIndex] + QString::fromUtf8("识别成功"));
            emit sigLoginSucceed(true);
            //停止识别
            StopIdent();
            return;
        }
    }
    std::lock_guard<std::mutex> lck (_identCapImgMutex, std::adopt_lock);

    if(!_isClose && _isLoginIdent)
    {
        //异步识别识别
        StartLoginIdent();
    }
}

/*****************************************************************************
*                        停止识别函数
*  函 数 名：StopIdent
*  功    能：停止识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::StopIdent()
{
    if(_isClose)
    {
        return;
    }
    _isClose = true;
    //int error = _identApi->StopIden();
    if(error != 0)
    {
        std::cout<<"停止识别失败！ "<<std::endl;
    }
}

/*****************************************************************************
*                        界面时间更新显示
*  函 数 名：timerEvent
*  功    能：界面时间更新显示
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2018-10-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::timerEvent(QTimerEvent *event)
{
    //获取系统现在的时间
    QDateTime time = QDateTime::currentDateTime();
    //设置系统时间显示格式
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
    //界面显示
    ui->labTime->setText(QString::fromUtf8("时间：")+ strTime + QString::fromUtf8("  帧率： ") + QString::number(_count));
    _count =0;
}


int count=1;
/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：ImageUpdate
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::slotImageUpdate(cv::Mat im)
{
    if(!im.empty())
    {

        if(count%5==0){
            _pFaceAlgorith->UpdateImage(im);
            count=0;
        }
        count++;

        //        QPixmap pixmap = QPixmap::fromImage(im);
        //        pixmap.scaled(ui->labRealTimeIm->size(), Qt::KeepAspectRatio);
        //        ui->labRealTimeIm->setScaledContents(true);
        //        ui->labRealTimeIm->setPixmap(pixmap);


        //        int h = im.rows();
        //        int w = im.cols();



        //        QImage  ims(w,h,QImage::Format_RGB32);
        QImage frame = cvMat2QImage(im);

        QPainter painter(&frame);
        //        //画想要画的东西
        //        painter.setPen(QPen(Qt::red,15,Qt::SolidLine,Qt::RoundCap,Qt::MiterJoin));
        //        painter.setBrush(Qt::NoBrush);
        //        QRect rect(100,100,250,200);//构造一个矩形
        painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
        painter.drawImage(0,0,_top_im);
        painter.end();
        QPixmap pixmap = QPixmap::fromImage(frame);
        pixmap.scaled(ui->labelFace->size(), Qt::KeepAspectRatio);
        ui->labelFace->setScaledContents(true);
        ui->labelFace->setPixmap(pixmap);
        //_count ++;
    }
    else
    {
        std::cout<<"识别显示未获得图像！"<<std::endl;
    }
}


/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：ImageUpdate
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogIdentify::IrisImageUpdate(QImage im)
{
    if(!im.isNull())
    {
        ui->labEye->setPixmap(QPixmap::fromImage(im));
        _count ++;
    }
    else
    {
        std::cout<<"识别显示未获得图像！"<<std::endl;
    }
}

