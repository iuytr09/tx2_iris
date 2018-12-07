#include "enrollworker.h"
#include "../JD_IRIS_API/include/jd_iris.h"
#include "Algorithm/iris_algorith.h"
#include <unistd.h>

EnrollWorker::EnrollWorker(QObject *parent) : QObject(parent),_isruning(false)
{
    am= IRIS_Algorith::GetInstance();
    pJD_IRIS =am->GetJD_IRIS();
    _eid=std::thread(EnrolThread, (void*)this);
}

EnrollWorker::~EnrollWorker()
{
    _isruning=false;
    _eid.join();
}

void * EnrollWorker::EnrolThread(void* arg){


    EnrollWorker* e =static_cast<EnrollWorker *>(arg);
    int nMaxLen;
    e->pJD_IRIS->JD_IRIS_GetFeatureLen(&nMaxLen);

    e->_isruning =true;
    while(e->_isruning){
        if(e->_ims.isEmpty()){
            usleep(500*1000);
        }else{
            cv::Mat im =  e->GetIM();
            if(im.empty())
            {
                usleep(500*1000);
                continue;
            }
            //声明存放定位结果的变量
            std::vector<st_EYE_LOC_INFO> EyeRect;
            JD_IRIS_STATUS nStatus = e->pJD_IRIS->JD_IRIS_EyeLocation(im, EyeRect);

            if (JD_STATUS_OK == nStatus){
                if(2 == EyeRect.size()){
                    //shuang yan
                    double dGlasses;
                    nStatus = e->pJD_IRIS->JD_IRIS_GlassesExist(im, EyeRect, dGlasses);

                    if(JD_STATUS_OK == nStatus && dGlasses<0.5){
                        JD_IRIS_TEMPLATE pTempCode1;
                        bool isok_t1=false;
                        JD_IRIS_TEMPLATE pTempCode2;
                        bool isok_t2=false;
                        double dQualityL=0.0;
                        double dQualityR=0.0;
#pragma omp parallel sections
                        {
#pragma omp section
                            {

                                //left
                                nStatus = e->pJD_IRIS->JD_IRIS_IrisQuality(im,EyeRect[0], dQualityL);
                                if(JD_STATUS_OK == nStatus && dQualityL>0.85){


                                    pTempCode1.nLength = 0;
                                    pTempCode1.pData = new unsigned char[nMaxLen];
                                    //虹膜特征提取
                                    if(JD_STATUS_OK == nStatus)
                                    {
                                        nStatus = e->pJD_IRIS->JD_IRIS_GetIrisFeatureCode(im,EyeRect[0], &pTempCode1);
                                        if(JD_STATUS_OK == nStatus)
                                        {
                                            isok_t1=true;

                                        }else{
                                            //                                            cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(255,0,0));
                                            //                                             getError(nStatus);
                                        }
                                    }else{
                                        //                                        cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(255,0,0));
                                        //                                         getError(nStatus);
                                    }
                                }else{
                                    //                                    cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(255,0,0));
                                    //                                     getError(nStatus);
                                }
                            }
#pragma omp section
                            {

                                //right
                                nStatus = e->pJD_IRIS->JD_IRIS_IrisQuality(im,EyeRect[1], dQualityR);

                                pTempCode2.nLength = 0;
                                pTempCode2.pData = new unsigned char[nMaxLen];
                                //虹膜特征提取.
                                if(JD_STATUS_OK == nStatus)
                                {
                                    nStatus = e->pJD_IRIS->JD_IRIS_GetIrisFeatureCode(im,EyeRect[1], &pTempCode2);
                                    if(JD_STATUS_OK == nStatus)
                                    {
                                        isok_t2=true;
                                    }else{
                                        // cv::rectangle(im,EyeRect[1].EyeRect,cv::Scalar(255,0,0));
                                        //                                             getError(nStatus);
                                    }
                                }else{
                                    //  cv::rectangle(im,EyeRect[1].EyeRect,cv::Scalar(255,0,0));
                                    //                                         getError(nStatus);
                                }
                            }
                        }




                        if(isok_t1&&isok_t2){
                            IRIS_Mat_Quality iris_im;
                            iris_im.im = im;
                            iris_im.leftQuality = dQualityL;
                            iris_im.rightQuality=dQualityR;
                            e->am->SaveFeature(&pTempCode1,&pTempCode2,iris_im);
                        }




                    }
                }
            }
        }
    }

    std::cout<<"---------------------------------xian cheng tui chu !!!!!!!!!!"<<std::endl;
}
