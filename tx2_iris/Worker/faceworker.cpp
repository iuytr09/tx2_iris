#include "faceworker.h"
#include "../jdFace_sdk_2.0.0/visi_face.h"
#include"Algorithm/face_algorith.h"
#include <unistd.h>
#include<sys/time.h>

//二. 人脸


//防伪阈值： 0.5

//识别阈值-
//---------------------------------------------
//阈值       误识率        通过率
//---------------------------------------------
//1.1367       0.0001       0.9999
//1.0819       0.00001      0.9997
//1.0335       0.000001     0.9984
//0.9738       0.0000001    0.9937

//建议识别阈值0.9738

FaceWorker::FaceWorker(QObject *parent) : QObject(parent)
{
    am= Face_Algorith::GetInstance();
    _fs=FaceState::FaceUnknown;
    _eid=std::thread(FaceThread, (void*)this);
}

FaceWorker::~FaceWorker()
{
    _isruning=false;
    _eid.join();
}

void FaceWorker::Run(){
    if(!_isruning){
        _eid=std::thread(FaceThread, (void*)this);
    }
}
/*****************************************************************************
*                        函数#include<sys/time.h>
*  函 数 名：
*  功    能： 功能描述
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
void * FaceWorker::FaceThread(void* arg){


    FaceWorker* e =static_cast<FaceWorker *>(arg);
    e->_isruning =true;
    while(e->_isruning){

        if(e->_isReady){
            cv::Mat im;
            QMutexLocker locker(& e->_face_mutex);
            e->_isReady = false;
            im= e->_im.clone();
            locker.unlock();
            if(im.empty())
            {
                usleep(100*1000);
                continue;
            }

            if(e->_fs==FaceState::FaceUnknown){
                usleep(5*1000);
            }else if(e->_fs==FaceState::FaceIdent){

                std::cout<<"FaceIdent"<<std::endl;

                std::vector<std::vector<float>> face_boxs;
                std::vector<cv::Mat> out_faces;


                struct timeval tv,etv;
                struct timezone tz,etz;
                gettimeofday(&tv,&tz);

                int state = get_all_faces(im,face_boxs,out_faces);

                gettimeofday(&etv,&etz);
                std::cout<<"get_all_faces :  "<<etv.tv_sec*1000 + etv.tv_usec/1000- tv.tv_sec*1000 - tv.tv_usec/1000<<"ms"<<std::endl;

                if(state==0){
                    //emit e->am->sigIdentState(1,face_boxs);
                    std::vector<float> fbox;
                    std::vector<float> feat;

                    struct timeval tv,etv;
                    struct timezone tz,etz;
                    gettimeofday(&tv,&tz);

                    state= extract_feature(im, fbox,feat,2);

                    gettimeofday(&etv,&etz);
                    std::cout<<"extract_feature:  "<<etv.tv_sec*1000 + etv.tv_usec/1000- tv.tv_sec*1000 - tv.tv_usec/1000<<"ms"<<std::endl;

                    locker.unlock();
                    if(state==0)
                    {
                        e->am->CodeCompare(feat);
                    }else{
                        emit e->am->sigFaceState(InteractionResultType::FaceIdenforming,e->am->GetTip(state));
                    }
                }else{
                    emit e->am->sigFaceState(InteractionResultType::FaceIdenforming,e->am->GetTip(state));
                }



            }else if(e->_fs==FaceState::FaceEnroll){

                std::cout<<"FaceEnroll"<<std::endl;
                std::vector<float> fbox;
                cv::Mat reg_face;
                std::vector<float> feat;


                struct timeval tv,etv;
                struct timezone tz,etz;
                gettimeofday(&tv,&tz);

                int state = face_register(im, fbox, reg_face, feat,2);

                gettimeofday(&etv,&etz);
                std::cout<<"face_register:  "<<etv.tv_sec*1000 + etv.tv_usec/1000- tv.tv_sec*1000 - tv.tv_usec/1000<<"ms"<<std::endl;

                if(state==0){

                    struct timeval tv,etv;
                    struct timezone tz,etz;
                    gettimeofday(&tv,&tz);

                    if(e->am->SaveFeature(feat,im,reg_face)){
                        //                       e->_fs = FaceState::FaceUnknown;
                        //                       emit  e->am->sigEnrollState(0,fbox,reg_face);//成功
                        emit e->am->sigFaceState(InteractionResultType::FaceEnrollSuccess,IrisPositionFlag::Unknown);
                    }else{
                        //                       emit  e->am->sigEnrollState(-10,fbox,reg_face); //存储失败
                        emit e->am->sigFaceState(InteractionResultType::FaceEnrollFailed,IrisPositionFlag::Unknown);
                    }
                    gettimeofday(&etv,&etz);
                    std::cout<<"SaveFeature :  "<<etv.tv_sec*1000 + etv.tv_usec/1000- tv.tv_sec*1000 - tv.tv_usec/1000<<"ms"<<std::endl;

                }else{
                    emit e->am->sigFaceState(InteractionResultType::FaceEnrollPerformig,e->am->GetTip(state));
                }


            }else{
                usleep(1000);
            }
        }else{
            usleep(1000);
        }

    }

}

