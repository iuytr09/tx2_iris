#include "face_algorith.h"
#include<QSqlRecord>
#include<QMessageBox>
#include<QSqlQuery>
#include<QSqlError>
#include<QVariant>
#include "dbconnection.h"

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
    QMutexLocker locker(&_save_im_mutex);//加互斥锁。

    QSqlQuery query;
    query.prepare("INSERT INTO facedata(id,uid,name,depart_name,feat) VALUES (NULL,:uid,:name,:depart_name,:feat)");

    query.bindValue(":uid", _user.id);
    query.bindValue(":name", _user.name);
    query.bindValue(":depart_name", _user.depart_name);

    QByteArray array;

    //int l=sizeof(feat); // 4*4 = 16 (一个float占4个字节)
    int len_fVar =  4*feat.size();//
    array.resize(len_fVar);
    memcpy(array.data(), &feat[0], len_fVar);

    query.bindValue(":feat",  array);
    query.exec();
    std::cout<<"人脸注册成功!!"<<std::endl;
    //更新缓存人脸特征
    FaceDataTP tp(0,_user.id,_user.name,_user.depart_name);
    tp._facetm = feat;
    _faceDataTPs.push_back(tp);

    if(!query.isActive())
    {
        QMessageBox::warning(NULL,tr("数据库存储人脸信息失败"),query.lastError().text());
    }else{
        std::cout<<"存储ren lian te zheng 信息成功!!"<<std::endl;
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
*  函 数 名： UpdateImage
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
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Face_Algorith::CodeCompare(std::vector<float> source){
    float dDist=0.0;
    FaceDataTP record;
    std::vector<std::vector<float>> face_box;
    for(int i=0;i<_faceDataTPs.size();i++){
        double dtemp=0.0;

        dtemp= get_score(_faceDataTPs.at(i)._facetm.data(), source.data());
        if(dtemp>dDist)
        {
            dDist = dtemp;
            record = _faceDataTPs.at(i);
        }
    }

    if(dDist>0.3){
        //匹配成功
        _user.id =record._uid;
        _user.name = record._name;
        _user.depart_name = record._depart_name;
        emit sigIdentSucsses(_user);//识别成功！
    }else{
        emit sigIdentState(-9,face_box);//识别失败！

    }
}
