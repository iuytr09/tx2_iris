#ifndef IRISVIDEO_H
#define IRISVIDEO_H
#include <GxIAPI/GxIAPI.h>
#include<GxIAPI/Fps.h>
#include <QObject>
#include<QMutexLocker>
#include<QMutex>
#include<QReadWriteLock>
//#include<QAtomicPointer>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<thread>
//hong wai deng
#include<GPIO/GPIOSet.h>

using namespace cv;
using namespace std;


/*****************************************************************************
*                        类
*  类    名：IrisVideo
*  功    能： 虹膜摄像头类
*  说    明： 获取虹膜图像
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
class IrisVideo: public QObject
{
    Q_OBJECT

public:
    /*! \brief 用于获得SingleTon实例，使用单例模式。
     *  \return SingleTon实例的引用。
     */
    static IrisVideo *GetInstance();


    void SetStopOrStart(){
        QMutexLocker loker(&_iris_v_mutex);
        _stoped = !_stoped;
        if(!_stoped){
            irisGxStreamOn();
        }else{
            irisStopStreamOff();
        }
    }

    bool GetStop(){
        return _stoped;
    }

    void SetStop(bool stop){
         _stoped =stop;
    }

    inline GX_STATUS GetStatus(){
        return _status;
    }

    void Run();

    bool GetRuning(){
        return _runing;
    }


signals:
    //信号
    void sigFramed(cv::Mat im);

private:
    IrisVideo(QObject *parent);//禁止构造函数。
    IrisVideo(const IrisVideo &);//禁止拷贝构造函数。
    IrisVideo & operator=(const IrisVideo &);//禁止赋值拷贝函数。
    ~IrisVideo();//禁止构造函数。

    //停止采集
    static void irisStopStreamOff();
    static void irisGxStreamOn();
    static void * GetImageThread(void* arg); //上传图片数据线程

    //QReadWriteLock _internalMutex;//函数使用的读写锁。
    static QMutex _iris_v_mutex;//实例互斥锁。
    static IrisVideo* _iris_v_instance;/*!<使用原子指针,默认初始化为0。*/

    GX_STATUS _status;
    std::thread _capImgThread;               //识别模块图像显示线程
    bool  _isOpen;
    bool _runing;
    GX_DEV_HANDLE _hDevice;
    uint64_t _nBufferNum;
    CFps _objFps;
    volatile bool _stoped;


    //gpio
    int gpio_out;
    int gpio_in;
};



#endif // IRISVIDEO_H
