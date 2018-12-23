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

void FaceWorker::Run(){
    if(!_isruning){
        _eid=std::thread(FaceThread, (void*)this);
    }
}

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
               std::vector<std::vector<float>> face_boxs;
               std::vector<cv::Mat> out_faces;
               int state = get_all_faces(im,face_boxs,out_faces);
               if(state==0){
                   emit e->am->sigIdentState(1,face_boxs);
                   std::vector<float> fbox;
                   std::vector<float> feat;

                   state= extract_feature(im, fbox,feat,2);
                   locker.unlock();
                   if(state==0)
                   {
                       e->am->CodeCompare(feat);
                   }
               }



           }else if(e->_fs==FaceState::FaceEnroll){

               std::vector<float> fbox;
               cv::Mat reg_face;
               std::vector<float> feat;
               int state = face_register(im, fbox, reg_face, feat,2);
               if(state==0){
                   if(e->am->SaveFeature(feat,im,reg_face)){
                       e->_fs = FaceState::FaceUnknown;
                       emit  e->am->sigEnrollState(0,fbox,reg_face);//cheng gong
                   }else{
                       emit  e->am->sigEnrollState(-10,fbox,reg_face); //cun chu shi bai
                   }
               }else{
                   emit  e->am->sigEnrollState(state,fbox,reg_face);
               }


           }else{
               usleep(50*1000);
           }
        }else{
             usleep(50*1000);
        }

    }

}

