#include "face_algorith.h"
#include<QSqlRecord>
#include<QMessageBox>
#include<QSqlQuery>
#include<QSqlError>
#include<QVariant>
#include "dbconnection.h"
#include <math.h>
#include<sys/time.h>
#include "../jdFace_sdk_2.0.0/visi_error.h"

//静态成员变量初始化。
QMutex Face_Algorith::_face_alg_mutex;
Face_Algorith* Face_Algorith::_face_alg_instance = NULL;

/*****************************************************************************
*                       构造函数
*  函 数 名： Face_Algorith
*  功    能： 人脸算法初始化
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
Face_Algorith::Face_Algorith():_pfaceWorker(NULL)
{
    if(createConnection(NULL))      //创建数据库连接
    {

        //初始化人脸算法
        _statue =init_visi_face_module("/home/nvidia/work/jdFace_sdk_2.0.0/models",1,1,1);
        _imagemanager.InitParam();

        //采用sql语句方式获取数据
        QSqlQuery query;
        query.exec("SELECT id ,uid,name,depart_name,feat FROM facedata GROUP BY uid ");

        while (query.next()) {
            FaceDataTP tp(query.value(0).toInt(),query.value(1).toInt(),query.value(2).toString(),query.value(3).toString());
            QByteArray datatemple=query.value("feat").toByteArray();
            tp._facetm.resize(1024);
            memcpy(&tp._facetm[0], datatemple.data(), 4*1024);
            _faceDataTPs.push_back(tp);
        }

        if(!query.isActive())
        {
            QMessageBox::warning(NULL,tr("数据库存储人脸信息失败"),query.lastError().text());
        }else{
            std::cout<<"du qu ren lian 信息成功!!"<<std::endl;
        }
    }
}



Face_Algorith::~Face_Algorith(){
    release_visi_face_module();
}

/*****************************************************************************
*                        函数
*  函 数 名： StartWorker
*  功    能： 启动人脸识别注册识别线程
*  说    明：如果该线程已存在则启动线程
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Face_Algorith::StartWorker()
{
    if (_pfaceWorker==NULL){
        _pfaceWorker= new FaceWorker();
    }else{
        _pfaceWorker->Run();
    }
}

/*****************************************************************************
*                        函数
*  函 数 名： SaveFeature
*  功    能： 存储人脸特征
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool Face_Algorith::SaveFeature(std::vector<float> &feat,cv::Mat im,cv::Mat reg_face)
{
    SetNoWork();
    QMutexLocker locker(&_save_im_mutex);//加互斥锁。

    QSqlQuery query;
    query.prepare("INSERT INTO facedata(id,uid,name,depart_name,feat) VALUES (NULL,:uid,:name,:depart_name,:feat)");

    query.bindValue(":uid", _user.id);
    query.bindValue(":name", _user.name);
    query.bindValue(":depart_name", _user.depart_name);

    QByteArray array;
    int len_fVar =  4*feat.size();
    array.resize(len_fVar);
    memcpy(array.data(), &feat[0], len_fVar);

    query.bindValue(":feat",  array);
    query.exec();
    std::cout<<"人脸注册成功!!"<<std::endl;
    //更新缓存人脸特征
    FaceDataTP tp(0,_user.id,_user.name,_user.depart_name);
    std::cout<<"存储人脸特征信息: id:"<<_user.id<<"  name:"<<_user.name.toStdString()<<std::endl;
    tp._facetm = feat;
    _faceDataTPs.push_back(tp);

    if(!query.isActive())
    {
        QMessageBox::warning(NULL,tr("数据库存储人脸信息失败"),query.lastError().text());
    }else{
        std::cout<<"存储人脸特征信息成功!!"<<std::endl;
    }
    //存储人脸图像
    _imagemanager.SaveFaceImage(_user.id,im,reg_face);
}

/*****************************************************************************
*                        SetIdentifyWork函数
*  函 数 名： SetIdentifyWork
*  功    能： 设置人脸识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Face_Algorith::SetIdentifyWork(){
    if(_pfaceWorker!=NULL){
        _pfaceWorker->SetState(FaceState::FaceIdent);
    }
}

/*****************************************************************************
*                        UpdateImage函数
*  函 数 名： UpdateImageFaceEnroll
*  功    能： 向算法线程传达图像
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
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

/*****************************************************************************
*                        SetEnrollWork函数
*  函 数 名： SetEnrollWork
*  功    能： 设置人脸注册
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Face_Algorith::SetEnrollWork(){

    if(_pfaceWorker!=NULL && _user.id>0){
        _pfaceWorker->SetState(FaceState::FaceEnroll);
    }else{
        std::cout<<"_user.id:"<<_user.id<<std::endl;
    }
}

/*****************************************************************************
*                       SetNoWork 函数
*  函 数 名： SetNoWork
*  功    能： 设置非工作模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Face_Algorith::SetNoWork(){

    if(_pfaceWorker!=NULL && _user.id>0){
        _pfaceWorker->SetState(FaceState::FaceUnknown);
    }else{
        std::cout<<"_user.id:"<<_user.id<<std::endl;
    }
}

/*****************************************************************************
*                        函数
*  函 数 名： CodeCompare
*  功    能： 特征比对函数
*  说    明：
* 防伪阈值： 0.5#include<sys/time.h>
*   识别阈值-
*   ---------------------------------------------
*   阈值       误识率        通过率
*   ---------------------------------------------
*   1.1367       0.0001       0.9999
*   1.0819       0.00001      0.9997
*   1.0335       0.000001     0.9984
*   0.9738       0.0000001    0.9937
*   建议识别阈值0.9738                  目前设置0.3
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Face_Algorith::CodeCompare(std::vector<float> source){
    float dDist=2.0;
    FaceDataTP record;
    struct timeval tv,etv;
    struct timezone tz,etz;
    std::vector<std::vector<float>> face_box;
    for(int i=0;i<_faceDataTPs.size();i++){
        double dtemp = 2.0;


        gettimeofday(&tv,&tz);

        dtemp = get_score(_faceDataTPs.at(i)._facetm.data(), source.data());

        gettimeofday(&etv,&etz);
        std::cout<<"get_score:  "<<etv.tv_sec*1000 + etv.tv_usec/1000- tv.tv_sec*1000 - tv.tv_usec/1000<<"ms"<<std::endl;
        dtemp = sqrt(2-2*dtemp);
        gettimeofday(&etv,&etz);
        std::cout<<"sqrt:  "<<etv.tv_sec*1000 + etv.tv_usec/1000- tv.tv_sec*1000 - tv.tv_usec/1000<<"ms"<<std::endl;

        if(dtemp<dDist)
        {
            dDist = dtemp;
            record = _faceDataTPs.at(i);
        }
    }

    std::cout<<"dDist:"<<dDist<<std::endl;

    if(dDist<1.1367){
        PersonInfo user;
        //匹配成功
        user.id =record._uid;
        user.name = record._name;
        user.depart_name = record._depart_name;
        emit sigIdentSucsses(user);//识别成功！
    }else{
        emit sigFaceState(InteractionResultType::FaceIdenFailed,IrisPositionFlag::Unknown);

    }
}


/*****************************************************************************
*                        函数
*  函 数 名：GetTip
*  功    能： 获取提示信息
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
IrisPositionFlag Face_Algorith::GetTip(int ret){
    switch (ret) {
    case ERR_FACE_LOST:
        return IrisPositionFlag::Unknown;
    case ERR_FACE_POSE_WRONG:
        return IrisPositionFlag::POSEWRONG;
    case ERR_FACE_MOTION_BLUR:
        return IrisPositionFlag::Quality;
    case ERR_FACE_TOO_FAR:
        return IrisPositionFlag::Far;
    case ERR_FACE_TOO_NEAR:
        return IrisPositionFlag::Near;
    case ERR_NOFACE:
        return IrisPositionFlag::Unknown;
    default:
        return IrisPositionFlag::Unknown;
    }
    return IrisPositionFlag::Unknown;
}
