#include "face_algorith.h"
#include<QSqlRecord>

//静态成员变量初始化。
QMutex Face_Algorith::_face_alg_mutex;
Face_Algorith* Face_Algorith::_face_alg_instance = NULL;


Face_Algorith::Face_Algorith()
{

    _statue =init_visi_face_module("/home/nvidia/work/jdFace_sdk_2.0.0/models",1,1,1);

    _pFaceDataModel = new QSqlTableModel();
    _pFaceDataModel->setTable("facedata");
    _pFaceDataModel->select();


    QSqlRecord record;
    for(int i=0;i<_pFaceDataModel->rowCount();i++){
        double dtemp=1.0;
        QSqlRecord recordtemp= _pFaceDataModel->record(i);
        // std::vector<float> codeExist;
        QByteArray datatemple=recordtemp.value("feat").toByteArray();
        std::vector<float> code;
        code.resize(1024);
        int len_fVar = sizeof(code);
        memcpy(&code, datatemple.data(), len_fVar);
        _vfaceExist.push_back(code);
    }
}

Face_Algorith::~Face_Algorith(){
    release_visi_face_module();
}

bool Face_Algorith::saveFeature(std::vector<float> &feat,cv::Mat im)
{
    QMutexLocker locker(&_save_im_mutex);//加互斥锁。
    QSqlRecord record = _pFaceDataModel->record();
    record.setValue("uid", _user.id);
    record.setValue("name", _user.name);
    record.setValue("depart_name", _user.depart_name);

    QByteArray array;
    int len_fVar = sizeof(feat); // 4*4 = 16 (一个float占4个字节)
    array.resize(len_fVar);
    memcpy(array.data(), &feat, len_fVar);

    QString tb = QString("feat");
    record.setValue(tb,  array);

    // _imagemanager.SaveImage(_user.id,index,_images.at(index).im);

    _pFaceDataModel->insertRecord(-1, record);
    bool isok =_pFaceDataModel->submitAll();

    if(isok){
        std::cout<<"cun chu cheng gong!!"<<std::endl;
        return true;
    }else{
        std::cout<<"cun chu shi bai !"<<std::endl;
        return false;
    }
    //emit sigEnrollSuccess();
}

void Face_Algorith::Identify(cv::Mat im){
    std::vector<std::vector<float>> face_boxs;
    std::vector<cv::Mat> out_faces;
    int state = get_all_faces(im,face_boxs,out_faces);
    if(state==0){
        std::vector<float> fbox;
        std::vector<float> feat;

        state= extract_feature(im, fbox,feat, 0.2);
        if(state==0)
        {
            codeCompare(feat);
        }
    }

    //float get_score(const float* feat_1, const float* feat_2);
}

void Face_Algorith::LoginIdentify(cv::Mat im){

    //     int get_all_faces(const cv::Mat &image, std::vector<std::vector<float>> &face_boxs, std::vector<cv::Mat> &out_faces);

    //     int extract_feature(const cv::Mat &image, std::vector<float> &fbox, std::vector<float> &feat, float hackness_thr = 0.2);
    //float get_score(const float* feat_1, const float* feat_2);
}

void Face_Algorith::Enroll(cv::Mat im){
    std::vector<std::vector<float>> face_boxs;
    std::vector<cv::Mat> out_faces;
    int state = get_all_faces(im,face_boxs,out_faces);
    if(state==0){
        std::vector<float> fbox;
        cv::Mat reg_face;
        std::vector<float> feat;
        state = face_register(im, fbox, reg_face, feat,0.2);
        if(state==0){
            if(saveFeature(feat,im)){
                emit sigEnrollSuccess(0);
            }else{
                emit sigEnrollSuccess(-2);
            }
        }else{
            emit sigEnrollSuccess(-1);
        }

    }



    int extract_feature(const cv::Mat &image, std::vector<float> &fbox, std::vector<float> &feat, float hackness_thr = 0.2);


}

void Face_Algorith::codeCompare(std::vector<float> source){
    float dDist=1.0;
    QSqlRecord record;
    for(int i=0;i<_pFaceDataModel->rowCount();i++){
        double dtemp=1.0;
//        QSqlRecord recordtemp= _pFaceDataModel->record(i);
//        // std::vector<float> codeExist;
//        QByteArray datatemple=recordtemp.value("feat").to;
        //codeExist->nLength = datatemple.length();

        //codeExist->pData = datatemple.data();
        dtemp= get_score(_vfaceExist.at(i).data(), source.data());
        if(dtemp<dDist)
        {
            dDist = dtemp;
            //record = recordtemp;
        }
    }

    //     if(dDist<0.32){
    //         //匹配成功
    //         //record  匹配结果
    //         _recogUser.id = record.value("uid").toInt();
    //         _recogUser.name = record.value("name").toString();
    //         _recogUser.depart_name = record.value("depart_name").toString();
    //         emit sigIdentifySuccess(PromptFlag::IRIS_SUCCESS);


    //     }else{
    //         emit sigIdentifySuccess(PromptFlag::IRIS_FAIL);

    //     }


}
