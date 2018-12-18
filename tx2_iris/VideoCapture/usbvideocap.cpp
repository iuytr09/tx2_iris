#include "usbvideocap.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include <QMutex>


QMutex UsbVideoCap::_face_v_mutex;
UsbVideoCap* UsbVideoCap::_face_v_instance = NULL;


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

void* UsbVideoCap::CapImgThread(void* arg)
{

    UsbVideoCap* uvc =static_cast<UsbVideoCap *>(arg);
    VideoCapture cap;
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

    Mat frame;

    bool stop = false;
    while (!stop)
    {
        cap >> frame;
        //QImage im = cvMat2QImage(frame);
        //uvc->updateImage(frame);
        emit uvc->OnUpdateImage(frame);
        usleep(100);
    }
    return (void*)0;
}

