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
    _imagemanager.InitParam();

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

void Face_Algorith::StartWorker()
{
    if (_pfaceWorker==NULL){
        _pfaceWorker= new FaceWorker();
    }else{
        _pfaceWorker->Run();
    }
}


bool Face_Algorith::SaveFeature(std::vector<float> &feat,cv::Mat im,cv::Mat reg_face)
{
    QMutexLocker locker(&_save_im_mutex);//加互斥锁。
    QSqlRecord record = _pFaceDataModel->record();
    record.setValue("uid", _user.id);
    record.setValue("name", _user.name);
    record.setValue("depart_name", _user.depart_name);

    QByteArray array;
    int l=sizeof(&feat); // 4*4 = 16 (一个float占4个字节)
    int len_fVar =  4*1024;//
    array.resize(len_fVar);
    memcpy(array.data(), &feat, len_fVar);

    QString tb = QString("feat");
    record.setValue(tb,  array);



    _pFaceDataModel->insertRecord(-1, record);
    bool isok =_pFaceDataModel->submitAll();
    _vfaceExist.push_back(feat);

    _imagemanager.SaveFaceImage(_user.id,im,reg_face);

    if(isok){
        std::cout<<"cun chu cheng gong!!"<<std::endl;
        return true;
    }else{
        std::cout<<"cun chu shi bai !"<<std::endl;
        return false;
    }
    //emit sigEnrollSuccess();
}

void Face_Algorith::SetIdentifyWork(){
    if(_pfaceWorker!=NULL){
        _pfaceWorker->SetState(FaceState::FaceIdent);
    }
}

void Face_Algorith::UpdateImage(cv::Mat im){
    if(_pfaceWorker!=NULL){
        _pfaceWorker->Push(im);
    }
}


//void Face_Algorith::LoginIdentify(cv::Mat im){

//    //     int get_all_faces(const cv::Mat &image, std::vector<std::vector<float>> &face_boxs, std::vector<cv::Mat> &out_faces);

//    //     int extract_feature(const cv::Mat &image, std::vector<float> &fbox, std::vector<float> &feat, float hackness_thr = 0.2);
//    //float get_score(const float* feat_1, const float* feat_2);
//}


void Face_Algorith::SetEnrollWork(){

    if(_pfaceWorker!=NULL && _user.id>0){
        _pfaceWorker->SetState(FaceState::FaceEnroll);
    }else{
        std::cout<<"_user.id:"<<_user.id<<std::endl;
    }
}


void Face_Algorith::SetNoWork(){

    if(_pfaceWorker!=NULL && _user.id>0){
        _pfaceWorker->SetState(FaceState::FaceUnknown);
    }else{
        std::cout<<"_user.id:"<<_user.id<<std::endl;
    }
}

void Face_Algorith::CodeCompare(std::vector<float> source){
    float dDist=1.0;
    QSqlRecord record;
    std::vector<std::vector<float>> face_box;
    for(int i=0;i<_pFaceDataModel->rowCount();i++){
        double dtemp=1.0;
        QSqlRecord recordtemp= _pFaceDataModel->record(i);
        //        // std::vector<float> codeExist;
        //        QByteArray datatemple=recordtemp.value("feat").to;
        //codeExist->nLength = datatemple.length();

        //codeExist->pData = datatemple.data();
        dtemp= get_score(_vfaceExist.at(i).data(), source.data());
        if(dtemp<dDist)
        {
            dDist = dtemp;
            record = recordtemp;
        }
    }

    if(dDist<0.32){
        //匹配成功
        _user.id = record.value("uid").toInt();
        _user.name = record.value("name").toString();
        _user.depart_name = record.value("depart_name").toString();
        emit sigIdentSuccess(0,face_box);//识别成功！
    }else{
        emit sigIdentSuccess(-9,face_box);//识别失败！

    }
}
