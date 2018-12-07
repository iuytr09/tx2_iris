#include "identworker.h"
#include "../JD_IRIS_API/include/jd_iris.h"
#include "Algorithm/iris_algorith.h"
#include <unistd.h>

//IRIS_Algorith* am= IRIS_Algorith::GetInstance();
//JD_IRIS* pJD_IRIS =am->GetJD_IRIS();

IdentWorker::IdentWorker(QObject *parent) : QObject(parent),_isruning(false)
{

    am= IRIS_Algorith::GetInstance();
    pJD_IRIS =am->GetJD_IRIS();
    _iid=std::thread(Indethread, (void*)this);
}

IdentWorker::~IdentWorker()
{
    _isruning=false;
    _iid.join();
}
void * IdentWorker::Indethread(void* arg){

    int nMaxLen;
     IdentWorker* e =static_cast<IdentWorker *>(arg);
    e->pJD_IRIS->JD_IRIS_GetFeatureLen(&nMaxLen);
    e->_isruning =true;
    while(e->_isruning){
        if(e->_ims.isEmpty()){
            usleep(50*1000);
        }else{
           cv::Mat im = e->GetIM();
           if(im.empty()){
                usleep(50*1000);
                continue;
           }

           std::vector<st_EYE_LOC_INFO> EyeRect;
           JD_IRIS_STATUS nStatus = e->pJD_IRIS->JD_IRIS_EyeLocation(im, EyeRect);

           if (JD_STATUS_OK == nStatus){
               double dQuality;
               if (EyeRect.size()>0){
                   //left
                   nStatus = e->pJD_IRIS->JD_IRIS_IrisQuality(im,EyeRect.at(0), dQuality);
                   if(JD_STATUS_OK == nStatus && dQuality>0.05){
                       JD_IRIS_TEMPLATE pTempCode;// = new JD_IRIS_TEMPLATE();
                       pTempCode.nLength = 0;
                       pTempCode.pData = new unsigned char[nMaxLen];
                       //虹膜特征提取
                       if(JD_STATUS_OK == nStatus)
                       {
                           nStatus = e->pJD_IRIS->JD_IRIS_GetIrisFeatureCode(im,EyeRect[0], &pTempCode);
                           if(JD_STATUS_OK == nStatus)
                           {
                               e->am->codeCompareL(&pTempCode);
                            }else{
//                               getError(nStatus);
                           }
                       }else{
//                           getError(nStatus);
                       }

                       delete[] pTempCode.pData;
                       pTempCode.pData =NULL;

                   }else{
//                       getError(nStatus);
                   }
               }

               dQuality=0.0;
               if(2 == EyeRect.size()){
                   //right
                   nStatus = e->pJD_IRIS->JD_IRIS_IrisQuality(im,EyeRect[1], dQuality);
                   if(JD_STATUS_OK == nStatus && dQuality>0.05){
                       JD_IRIS_TEMPLATE pTempCode ;//= new JD_IRIS_TEMPLATE();
                       pTempCode.nLength = 0;
                       pTempCode.pData = new unsigned char[nMaxLen];
                       //虹膜特征提取.
                       if(JD_STATUS_OK == nStatus)
                       {
                           nStatus = e->pJD_IRIS->JD_IRIS_GetIrisFeatureCode(im,EyeRect[1], &pTempCode);
                           if(JD_STATUS_OK == nStatus)
                           {
                               e->am->codeCompareR(&pTempCode);
                           }else{
//                               getError(nStatus);
                           }

                       }else{
//                           getError(nStatus);
                       }

                       delete[] pTempCode.pData;
                       pTempCode.pData =NULL;
                   }else{
//                       getError(nStatus);
                   }

                   //shi bai
                   //getError(nStatus);
//                   return;

               }else{
                   //dan yan ------xian bu kao lv
//                   getError(nStatus);
               }
           }else{
               //shi bai
//               getError(nStatus);
           }

        }
    }
}
