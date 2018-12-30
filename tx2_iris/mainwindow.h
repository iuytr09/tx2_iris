/*****************************************************************************
** 文 件 名：MainWindow.h
** 主 要 类：MainWindow
**
** Copyright (c) shanggushijue
**
** 创 建 人：刘中昌
** 创建时间：2018-10-13
**
** 修 改 人：
** 修改时间：
** 描  述:   主窗体
** 主要模块说明: 标题栏、工具栏，模块切换
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "dialogidentify.h"
#include "QtWidgets/QHBoxLayout"
#include "QtWidgets/QToolBar"
#include "QtWidgets/QToolButton"
#include "suspendbar.h"
#include "manager.h"
#include "dialogabout.h"
#include "shutdown.h"
#include "syssettings.h"
#include "datamanage.h"
#include "QTimer"
#include "personmanage.h"
#include "Algorithm/face_algorith.h"
#include "Algorithm/iris_algorith.h"
#include "VideoCapture/irisvideo.h"


class MngForm;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum WorkStatus { CloclIn = 0, ClockOut, Other};
    enum PageStatus {IdentityPage,ManagerPage,IrisLoginPage,PersonPage,SystermPage,ShutdownPage,DataManagerPage,HelpPage,OtherPage};
private slots:
//    void ImageUpdate(QImage im);     //界面图像更新
    //上班打卡
    void on_start_work_action_triggered();

    // 管理员用户名登陆
    void on_login_for_admin_action_triggered();

    //关于对话框
    void on_about_action_triggered();

    //管理员虹膜登陆
    void go_irislogin_action();

    //识别界面
    void go_identify_action();
\
    //管理界面
    void go_manager_action();

    //数据管理界面
    void go_data_action();

    //人员管理界面
    void go_person_action();

    //系统管理界面
    void go_systerm_action();

    //关机界面
    void go_shutdown_action();

    //帮助界面
    void go_help_action();

    //登陆是否成功
    void login_result(bool isSucceed);
    //隐藏登陆按钮栏
    void hide_suspendbar();

public slots:
    void iris_image(cv::Mat im);
//    void face_image(cv::Mat im);
    //void iris_enroll_success(PersonInfo p);
    void iris_ident_success(PersonInfo p);
private :
    void mousePressEvent(QMouseEvent *);
    void changeInterface(QWidget *page);
    void onStackWidgetCurrentChanged(int index);
private:
    Ui::MainWindow *ui;

    DialogIdentify *_ident;//识别界面
    QHBoxLayout *_layoutIdentify;

    WorkStatus _workStatus;
    SuspendBar  *_suspendbar;

    Manager    *_manager;
    QHBoxLayout *_layoutManager;

    PersonManage *_personManage;
    QHBoxLayout *_layoutPerson;

    DialogAbout *_dialogAbout;
    QHBoxLayout *_layoutDialogAbout;

    Shutdown    *_shutdown;
    QHBoxLayout *_layoutShutdown;

    SysSettings  *_systermSet;
    QHBoxLayout *_layoutSystermSet;

    DataManage  *_dataManage;
    QHBoxLayout *_layoutDataManage;

    PageStatus _currentPage;

    QTimer *_timerForSuspendBar;

    bool    _loginShowTag;

    IrisVideo *_pIrisVideo;//hongmo采集
    IRIS_Algorith* _pAlgorith;
    Face_Algorith* _pFaceAlgorith;
//    static void * CapImgThread(void* arg); //上传图片数据线程
//    std::thread _capImgThread;            //识别模块图像显示线程

    //UsbVideoCap *_usbVideoCap;
};

#endif // MAINWINDOW_H
