/*****************************************************************************
** 文 件 名：dialogenroll.cpp
** 主 要 类：DialogEnroll
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   注册对话框
** 主要模块说明: 注册过程中的对话框处理
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "dialogenroll.h"
#include "ui_dialogenroll.h"

/*****************************************************************************
*                         DialogEnroll构造函数
*  函 数 名：DialogEnroll
*  功    能：分配DialogEnroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
DialogEnroll::DialogEnroll(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEnroll)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, 0, 800, 600));
    //setBackgroundColor(QColor(242,241,240));
}

/*****************************************************************************
*                         DialogEnroll析构函数
*  函 数 名：~DialogEnroll
*  功    能：释放DialogEnroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
DialogEnroll::~DialogEnroll()
{
    delete ui;
}

/*****************************************************************************
*                         DialogEnroll返回
*  函 数 名：on_btnDlgEnrollReturn_clicked
*  功    能：从DialogEnroll界面返回到Enroll界面
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogEnroll::on_btnDlgEnrollReturn_clicked()
{
    close();
}

/*****************************************************************************
*                         传递注册结果信息
*  函 数 名：XferIrisInfo
*  功    能：从Enroll界面传递注册结果信息给DialogEnroll界面
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2018-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogEnroll::XferIrisInfo(QImage resultImg,double qualityScore)
{
    resultImg = resultImg.scaled(640,480,Qt::KeepAspectRatio);



    if(qualityScore < MinEnrollQualityScore)
    {
        ui->lineEditQualityScore->setStyleSheet("color: rgb(255, 0, 0);");
    }
    else
    {
        ui->lineEditQualityScore->setStyleSheet("color: rgb(23, 111, 11);");
    }

    ui->lineEditQualityScore->setText( QString::number(qualityScore));

    ui->labIrisIm->setPixmap(QPixmap::fromImage(resultImg));

}
