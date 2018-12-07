#ifndef USBVIDEOCAP_H
#define USBVIDEOCAP_H
#include "./Common/common.h"
#include <thread>

class UsbVideoCap : public QObject
{
    Q_OBJECT
public:
    explicit UsbVideoCap(QObject *parent = 0);
    ~UsbVideoCap();
signals:


public slots:
private:
      static void * CapImgThread(void* arg); //上传图片数据线程
      std::thread _capImgThread;            //识别模块图像显示线程
      bool  _enableCapture;
};

#endif // USBVIDEOCAP_H
