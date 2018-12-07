#include "usbvideocap.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>

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
    VideoCapture cap(0);
    if(!cap.isOpened())
    {
        uvc->_enableCapture = false;
        return -1;
    }
    uvc->_enableCapture =true;
    Mat frame;
    while (uvc->_enableCapture)
    {
        cap >> frame;
        QImage im = cvMat2QImage(frame);
        usleep(30);
    }


    return (void*)0;
}

