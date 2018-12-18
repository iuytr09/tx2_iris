#include "faceworker.h"
#include "../jdFace_sdk_2.0.0/visi_face.h"
#include"Algorithm/face_algorith.h"
#include <unistd.h>


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

void * FaceWorker::FaceThread(void* arg){


    FaceWorker* e =static_cast<FaceWorker *>(arg);
    e->_isruning =true;
    while(e->_isruning){
        if(e->_fs==FaceState::FaceUnknown||e->_ims.isEmpty()){
            usleep(500*1000);
        }else if(FaceState::FaceIdent){
            cv::Mat im =  e->GetIM();
            if(im.empty())
            {
                usleep(500*1000);
                continue;
            }

            std::vector<std::vector<float>> face_boxs;
            std::vector<cv::Mat> out_faces;
            int state = get_all_faces(im,face_boxs,out_faces);
            if(state==0){
               // emit sigIdentState(1,face_boxs,out_faces);
                std::vector<float> fbox;
                std::vector<float> feat;

                state= extract_feature(im, fbox,feat, 0.2);
                if(state==0)
                {
                    e->am->CodeCompare(feat);
                }
            }


        }else if(FaceState::FaceEnroll){

            cv::Mat im =  e->GetIM();
            if(im.empty())
            {
                usleep(500*1000);
                continue;
            }
            std::vector<float> fbox;
            cv::Mat reg_face;
            std::vector<float> feat;
            int state = face_register(im, fbox, reg_face, feat,1);
            if(state==0){
                if(e->am->SaveFeature(feat,im,reg_face)){
                    emit  e->am->sigEnrollSuccess(0,fbox,reg_face);//cheng gong
                }else{
                    emit  e->am->sigEnrollSuccess(-10,fbox,reg_face); //shi bai
                }
            }else{
                emit  e->am->sigEnrollSuccess(state,fbox,reg_face);
            }


        }else{
            usleep(500*1000);
        }
    }

}

