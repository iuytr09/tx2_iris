#include "irisvideo.h"
#include "GxIAPI/GxIAPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

//静态成员变量初始化。
QMutex IrisVideo::_iris_v_mutex;
IrisVideo* IrisVideo::_iris_v_instance = NULL;


static IrisVideo *IrisVideo::GetInstance()
{
    /*! testAndSetOrders操作保证在原子操作前和后的的内存访问
     * 不会被重新排序。
     */
    if(_iris_v_instance==NULL)//第一次检测
    {
        QMutexLocker locker(&_iris_v_mutex);//加互斥锁。

        _iris_v_instance= new IrisVideo(NULL);
    }

    return _iris_v_instance;
}
/*****************************************************************************
*                       构造 函数
*  函 数 名： IrisVideo
*  功    能： 虹膜摄像头数据获取类
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
IrisVideo::IrisVideo(QObject *parent) : QObject(parent),_nBufferNum(5),_hDevice(NULL),_isOpen(false),_stoped(true),_runing(false)
{
    int device_num=0;

    _status = GX_STATUS_SUCCESS;

    //初始化库
    _status = GXInitLib();
    if(_status == GX_STATUS_SUCCESS)
    {
        printf("GXUpdateDeviceList(&device_num, 1000)\n");
        //更新设备列表
        _status = GXUpdateDeviceList(&device_num, 1000);

        //获取枚举设备个数
        if(_status == GX_STATUS_SUCCESS)
        {
            if(device_num <= 0)
            {
                printf("<No device>\n");
                _status = GXCloseLib();

            }else
            {
                printf("DeviceNum is: %d\n", device_num);
                //打开第一个设备
                _status = GXOpenDeviceByIndex(0, &_hDevice);
                printf("Open Device status = %d\n",_status);
                if(_status == GX_STATUS_SUCCESS){
                    _isOpen = true;

                    //设置采集队列Buffer个数
                    GXSetAcqusitionBufferNumber(_hDevice, _nBufferNum);

                    //设置URB大小和个数
                    GXSetInt(_hDevice, GX_DS_INT_STREAM_TRANSFER_SIZE, (64*1024));
                    GXSetInt(_hDevice, GX_DS_INT_STREAM_TRANSFER_NUMBER_URB, 64);


                    // 设置曝光,单位是us
                    int32_t i32ExposureTime = 50000;
                    size_t   iSize = 0;
                    iSize = sizeof(int32_t);

                    GXReadRemoteDevicePort(_hDevice, 0x00900038, &i32ExposureTime, &iSize);
                    printf("i32ExposureTime = %d\n",i32ExposureTime);


                    i32ExposureTime = 50000;

                    int status = GXWriteRemoteDevicePort(_hDevice, 0x00900038, &i32ExposureTime, &iSize);
                    if(status != GX_STATUS_SUCCESS)
                    {
                        printf("Set Exposure time Failed ret= %d\n",status);
                    }

                    //she zhe hong wai
                    this->gpio_out = 38;
                    gpio_export(this->gpio_out);
                    gpio_set_dir(this->gpio_out, "out");
                    gpio_set_value(this->gpio_out, LOW);

                }
            }
        }else{
            _status = GXCloseLib();
        }
    }
}

IrisVideo::~IrisVideo()
{
    _iris_v_instance->SetStop(true);
    _capImgThread.join();

    //关闭设备
    GXCloseDevice(_hDevice);

    //释放库
    GXCloseLib();
    printf("Exit!\n");
    _iris_v_instance =NULL;

}


/*****************************************************************************
*                        函数
*  函 数 名： irisStopStreamOff
*  功    能： 停止虹膜摄像头采集数据
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IrisVideo::irisStopStreamOff()
{

    if(_iris_v_instance->_hDevice!=NULL){
        //ting zhi hong wai
         gpio_set_value(this->gpio_out, LOW);
        //停止采集
        _iris_v_instance->_status = GxStreamOff(_iris_v_instance->_hDevice);
        printf("GxStreamOff Failed ret= %d\n",_iris_v_instance->_status);
        //guan deng
    }

}

/*****************************************************************************
*                        函数
*  函 数 名： irisGxStreamOn
*  功    能： 启动虹膜摄像头采集图像
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IrisVideo::irisGxStreamOn()
{
    if(_iris_v_instance->_hDevice!=NULL){
        //kai qi hong wei
         gpio_set_value(this->gpio_out, HIGH);
        //开始采集
        _iris_v_instance->_status = GxStreamOn(_iris_v_instance->_hDevice);
        printf("GxStreamOn Failed ret= %d\n",_iris_v_instance->_status);
        //kai deng
    }

}

/*****************************************************************************
*                        函数
*  函 数 名： Run
*  功    能： 启动采集线程
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IrisVideo::Run(){
    if(!_runing){
        _capImgThread = std::thread(GetImageThread, (void*)this);
    }
}

/*****************************************************************************
*                        函数
*  函 数 名： GetImageThread
*  功    能： 虹膜图像采集线程
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* IrisVideo::GetImageThread(void* arg)
{
    IrisVideo* uvc =static_cast<IrisVideo *>(arg);
    if (uvc->_status != GX_STATUS_SUCCESS)
    {
        printf("AcqusitionStart Failed ret= %d\n",uvc->_status);
        return (void*)0;
    }

    uvc->irisGxStreamOn();
    if (uvc->_status != GX_STATUS_SUCCESS)
    {
        printf("AcqusitionStart Failed ret= %d\n",uvc->_status);
        return (void*)0;
    }
    /*******************模拟采集********************************/
    //模拟采集数据
    usleep(500*1000);
    uvc->_runing = true;
    int i=680;
    while(uvc->_runing){
        if(i>1141){
            i=1;
        }
        QString path=  QString("/home/nvidia/work/tx2-gjh/raw_%1.jpg").arg(i);
        cv::Mat im = cv::imread(path.toStdString(),CV_LOAD_IMAGE_GRAYSCALE);

        emit uvc->sigFramed(im);
        i++;
        //"raw_1.jpg"+
        usleep(10*1000);
    }
    /*********************** end ****************************/

    //真实采集
    Mat reviMat = Mat(2064, 3088, CV_8UC1);
    uvc->_runing = true;
    while(uvc->_runing){
        usleep(50*1000);
        GX_FRAME_DATA *pFrameData = NULL;
        uvc->_status = GxDQBuf(uvc->_hDevice, &pFrameData, 1000);
        if(uvc->_status != GX_STATUS_SUCCESS){
            usleep(300);
            printf("虹膜数据采集失败！ ret= %d\n",uvc->_status);
            continue;
        }

        if(pFrameData != NULL)
        {
            cv::Mat im;
            memcpy(reviMat.data, pFrameData->pImgBuf, pFrameData->nImgSize);
            emit uvc->sigFramed(reviMat);
            //将图像buffer放回库中
            GxQBuf(uvc->_hDevice,pFrameData);
        }
        else
        {
            printf("虹膜数据采集失败\n");
        }
    }
    uvc->_runing= false;
    return (void*)0;
}
