#include "usbvideocap.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include <QMutex>



QMutex UsbVideoCap::_face_v_mutex;
UsbVideoCap* UsbVideoCap::_face_v_instance = NULL;

/*****************************************************************************
*                        函数
*  函 数 名： GetInstance
*  功    能： 人脸采集单例实现
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
static UsbVideoCap *UsbVideoCap::GetInstance()
{
    /*! testAndSetOrders操作保证在原子操作前和后的的内存访问
     * 不会被重新排序。
     */
    if(_face_v_instance==NULL)//第一次检测
    {
        QMutexLocker locker(&_face_v_mutex);//加互斥锁。

        _face_v_instance= new UsbVideoCap(NULL);
    }

    return _face_v_instance;
}


UsbVideoCap::UsbVideoCap(QObject *parent) : QObject(parent)
{
    _capImgThread = std::thread(CapImgThread, (void*)this);
}

UsbVideoCap::~UsbVideoCap()
{
    _enableCapture =false;
    _capImgThread.join();
}

/*****************************************************************************
*                        函数
*  函 数 名： CapImgThread
*  功    能： 人脸采集线程
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* UsbVideoCap::CapImgThread(void* arg)
{
    UsbVideoCap* uvc =static_cast<UsbVideoCap *>(arg);
    VideoCapture cap;
    //三个摄像头只要打开任何一个均可
    if(!cap.open(0))
    {
        if(!cap.open(1))
        {
            if(!cap.open(2))
            {
                return -1;
            }
        }
    }

    //设置摄像头分辨率
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920.0);//设置摄像头采集图像分辨率
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080.0);

    std::cout<<"set CV_CAP_PROP_FPS:"<<cap.set( CV_CAP_PROP_FPS, 10)<<std::endl;

      std::cout<<"CV_CAP_PROP_FPS:"<<cap.get(CV_CAP_PROP_FPS)<<std::endl;
  std::cout<<"CV_CAP_PROP_FRAME_WIDTH:"<<cap.get(CV_CAP_PROP_FRAME_WIDTH)<<std::endl;
  std::cout<<"CV_CAP_PROP_FRAME_HEIGHT:"<<cap.get(CV_CAP_PROP_FRAME_HEIGHT)<<std::endl;

    Mat frame;

    bool stop = false;
    while (!stop)
    {
        cap >> frame;
        emit uvc->OnUpdateImage(frame);
        usleep(100);
    }
    return (void*)0;
}

