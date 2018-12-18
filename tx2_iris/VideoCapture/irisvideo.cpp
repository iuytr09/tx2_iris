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

void IrisVideo::irisStopStreamOff()
{

    if(_iris_v_instance->_hDevice!=NULL){
        //停止采集
        _iris_v_instance->_status = GxStreamOff(_iris_v_instance->_hDevice);
        printf("GxStreamOff Failed ret= %d\n",_iris_v_instance->_status);
        //guan deng
    }

}

void IrisVideo::irisGxStreamOn()
{
    if(_iris_v_instance->_hDevice!=NULL){
        //开始采集
        _iris_v_instance->_status = GxStreamOn(_iris_v_instance->_hDevice);
         printf("GxStreamOn Failed ret= %d\n",_iris_v_instance->_status);
        //kai deng
    }

}

void IrisVideo::Run(){
    if(!_runing){
        _capImgThread = std::thread(GetImageThread, (void*)this);
    }
}

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

     Mat reviMat = Mat(2064, 3088, CV_8UC1);

 uvc->_runing = true;
    while(uvc->_runing){

//        if(uvc->_stoped){
//            usleep(500*1000);
//            continue;
//        }
        usleep(50*1000);
        GX_FRAME_DATA *pFrameData = NULL;
        uvc->_status = GxDQBuf(uvc->_hDevice, &pFrameData, 1000);
        if(uvc->_status != GX_STATUS_SUCCESS){
            usleep(300);
            printf("GxDQBuf Failed ret= %d\n",uvc->_status);
            continue;
        }

        if(pFrameData != NULL)
        {
            cv::Mat im;
           // std::cout<<" pFrameData->nHeight:"<<pFrameData->nHeight<<"  pFrameData->nWidth:"<< pFrameData->nWidth<<std::endl;
           // std::cout<<" pFrameData->nImgSize:"<<pFrameData->nImgSize<<std::endl;
//            im=cv::Mat(pFrameData->nHeight, pFrameData->nWidth, CV_LOAD_IMAGE_COLOR, (void*)pFrameData->pImgBuf);
//            im.
             memcpy(reviMat.data, pFrameData->pImgBuf, pFrameData->nImgSize);
             emit uvc->sigFramed(reviMat);
            //将图像buffer放回库中
            GxQBuf(uvc->_hDevice,pFrameData);
        }
        else
        {
            printf("GxDQBuf failed&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
        }
    }
    uvc->_runing= false;
    return (void*)0;
}
