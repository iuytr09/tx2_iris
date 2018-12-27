#ifndef USBVIDEOCAP_H
#define USBVIDEOCAP_H
#include "./Common/common.h"
#include <thread>
#include<QMutex>

class UsbVideoCap : public QObject
{
    Q_OBJECT


public:
    /*! \brief 用于获得SingleTon实例，使用单例模式。
     *  \return SingleTon实例的引用。
     */
    static UsbVideoCap *GetInstance();
signals:
    void OnUpdateImage(cv::Mat im);

private:

    void updateImage(cv::Mat im){
        emit OnUpdateImage(im);
    }

    UsbVideoCap(QObject *parent);//禁止构造函数。
    UsbVideoCap(const UsbVideoCap &);//禁止拷贝构造函数。
    UsbVideoCap & operator=(const UsbVideoCap &);//禁止赋值拷贝函数。
    ~UsbVideoCap();//禁止构造函数。
//     UsbVideoCap(QObject *parent = 0);
//    ~UsbVideoCap();


public slots:
private:
      static void * CapImgThread(void* arg); //上传图片数据线程
      std::thread _capImgThread;            //识别模块图像显示线程
      bool  _enableCapture;
      //QReadWriteLock _internalMutex;//函数使用的读写锁。
      static QMutex _face_v_mutex;//实例互斥锁。
      static UsbVideoCap* _face_v_instance;/*!<使用原子指针,默认初始化为0。*/
};

#endif // USBVIDEOCAP_H
