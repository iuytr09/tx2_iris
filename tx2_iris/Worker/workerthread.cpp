#include "workerthread.h"
#include <QSqlRecord>
#include "../JD_IRIS_API/include/jd_iris.h"
#include "Algorithm/iris_algorith.h"

//IRIS_Algorith* am= IRIS_Algorith::GetInstance();
//JD_IRIS* dev =am->GetJD_IRIS();


WorkerThread::WorkerThread(QObject *parent) :
    QThread(parent),_isruning(false){

}

//void WorkerThread::run(){
//    _isruning =ture;
//   // IRIS_Algorith* am= IRIS_Algorith.GetInstance();
//   // JD_IRIS* dev =am.GetJD_IRIS();
//    if(_type==RunType::Enroll){
//        while(_isruning){

//        }
//    }else if (_type==RunType::Ident){
//        while(_isruning){

//        }
//    }else if(_type== RunType::LoginIdent){
//        while(_isruning){

//        }
//    }

//}

//void WorkerThread::enroll(){

//    JD_IRIS_TEMPLATE pTempCode1;
//    bool isok_t1=false;
//    JD_IRIS_TEMPLATE pTempCode2;
//    bool isok_t2=false;
//    double dQualityL=0.0;
//    double dQualityR=0.0;
//#pragma omp parallel sections
//    {
//#pragma omp section
//        {

//            //left
//            nStatus = _pJD_IRIS->JD_IRIS_IrisQuality(im,EyeRect[0], dQualityL);
//            if(JD_STATUS_OK == nStatus && dQualityL>0.85){
//                int nMaxLen;
//                _pJD_IRIS->JD_IRIS_GetFeatureLen(&nMaxLen);

//                pTempCode1.nLength = 0;
//                pTempCode1.pData = new unsigned char[nMaxLen];
//                //虹膜特征提取
//                if(JD_STATUS_OK == nStatus)
//                {
//                    nStatus = _pJD_IRIS->JD_IRIS_GetIrisFeatureCode(im,EyeRect[0], &pTempCode1);
//                    if(JD_STATUS_OK == nStatus)
//                    {
//                        isok_t1=true;

//                    }else{
//                        cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(255,0,0));
//                        getError(nStatus);
//                    }
//                }else{
//                    cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(255,0,0));
//                    getError(nStatus);
//                }
//            }else{
//                cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(255,0,0));
//                getError(nStatus);
//            }
//        }
//#pragma omp section
//        {

//            //right
//            nStatus = _pJD_IRIS->JD_IRIS_IrisQuality(im,EyeRect[1], dQualityR);
//            if(JD_STATUS_OK == nStatus && dQualityR>0.85){
//                int nMaxLen;
//                _pJD_IRIS->JD_IRIS_GetFeatureLen(&nMaxLen);
//                JD_IRIS_TEMPLATE pTempCode ;//= new JD_IRIS_TEMPLATE();
//                pTempCode2.nLength = 0;
//                pTempCode2.pData = new unsigned char[nMaxLen];
//                //虹膜特征提取.
//                if(JD_STATUS_OK == nStatus)
//                {
//                    nStatus = _pJD_IRIS->JD_IRIS_GetIrisFeatureCode(im,EyeRect[1], &pTempCode2);
//                    if(JD_STATUS_OK == nStatus)
//                    {
//                        isok_t2=true;
//                    }else{
//                        cv::rectangle(im,EyeRect[1].EyeRect,cv::Scalar(255,0,0));
//                        getError(nStatus);
//                    }
//                }else{
//                    cv::rectangle(im,EyeRect[1].EyeRect,cv::Scalar(255,0,0));
//                    getError(nStatus);
//                }
//            }else{
//                cv::rectangle(im,EyeRect[1].EyeRect,cv::Scalar(255,0,0));
//                getError(nStatus);
//            }

//        }

//        if(isok_t1&&isok_t2){
//            //huan cun
//            _pleftT.push_back(pTempCode1);
//            _prightT.push_back(pTempCode2);

//            IRIS_Mat_Quality user_image;
//            user_image.im = im.clone();
//            user_image.leftQuality=dQualityL;
//            user_image.rightQuality=dQualityR;
//            _images.push_back(user_image);

//            cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(0,255,0));
//            cv::rectangle(im,EyeRect[1].EyeRect,cv::Scalar(0,255,0));

//        }else{

//            if(isok_t1){
//                cv::rectangle(im,EyeRect[0].EyeRect,cv::Scalar(0,255,0));
//            }
//            if(isok_t2){
//                cv::rectangle(im,EyeRect[1].EyeRect,cv::Scalar(0,255,0));
//            }
//        }
//    }

//}


////shi bie
//void WorkerThread::ident(JD_IRIS_TEMPLATE* source){
//    int index =-1;
//    double tD = 1.0;
//    QSqlTableModel* pIrisDataModel = am->GetIrisDataModel();
//    for(int i=0;i<pIrisDataModel->rowCount();i++){
//        double dDist=1.0;
//        QSqlRecord record= pIrisDataModel->record(i);
//        std::string datatemple=record.value("l_1_tc").toString().toStdString();
//        JD_IRIS_TEMPLATE* codeExist = new JD_IRIS_TEMPLATE();
//        codeExist->nLength = datatemple.length();
//        codeExist->pData = datatemple.c_str();
//        dev->JD_IRIS_CodeCompare(codeExist,source,dDist);
//        if(dDist<0.32)
//        {
//            if(tD<dDist){
//                tD = dDist;
//                index= i;
//            }
//        }
//    }

//    if(index>=0){
//         QSqlRecord record= pIrisDataModel->record(index);
//         PersonInfo recogUser;
//        recogUser.id = record.value("uid").toInt();
//        recogUser.name = record.value("name").toString();
//        recogUser.depart_name = record.value("depart_name").toString();
//        emit sigIdentifySuccess(recogUser);
//    }
//}


////login shi bie
////shibie   xiu gai ::IRIS_Algorith.GetIrisDataModel() de shu ju
//void WorkerThread::identLogin(JD_IRIS_TEMPLATE *source){
////    IRIS_Algorith* am= IRIS_Algorith::GetInstance();
////    JD_IRIS* dev =am->GetJD_IRIS();

//    int index =-1;
//    double tD = 1.0;
//    QSqlTableModel* pIrisDataModel = am->GetIrisDataModel();
//    for(int i=0;i<pIrisDataModel->rowCount();i++){
//        double dDist=1.0;
//        QSqlRecord record= pIrisDataModel->record(i);
//        std::string datatemple=record.value("l_1_tc").toString().toStdString();
//        JD_IRIS_TEMPLATE* codeExist = new JD_IRIS_TEMPLATE();
//        codeExist->nLength = datatemple.length();
//        codeExist->pData = datatemple.c_str();
//        dev->JD_IRIS_CodeCompare(codeExist,source,dDist);
//        if(dDist<0.32)
//        {
//            if(tD<dDist){
//                tD = dDist;
//                index= i;
//            }
//        }
//    }

//    if(index>=0){
//         QSqlRecord record= pIrisDataModel->record(index);
//         PersonInfo recogUser;
//        recogUser.id = record.value("uid").toInt();
//        recogUser.name = record.value("name").toString();
//        recogUser.depart_name = record.value("depart_name").toString();
//        emit sigIdentifySuccess(recogUser);
//    }
//}


