#ifndef FACE_ALGORITH_H
#define FACE_ALGORITH_H

#include <QObject>
#include<QMutexLocker>
#include<QSqlTableModel>
#include "Common/common.h"
#include "FeatureManage.h"
#include "../jdFace_sdk_2.0.0/visi_face.h"
#include "Worker/faceworker.h"



struct FaceDataTP
{
    FaceDataTP(){}
    FaceDataTP(int id,int uid,QString name,QString depart_name) {
        _id=id;
        _uid =uid;
        _name = name;
        _depart_name = depart_name;
    }
    int _id;
    int _uid;
    QString _name;
    QString _depart_name;
    std::vector<float> _facetm;

};


class Face_Algorith : public QObject
{
    Q_OBJECT
private:
    Face_Algorith();//禁止构造函数。
    ~Face_Algorith();
    Face_Algorith(const Face_Algorith &);//禁止拷贝构造函数。
    Face_Algorith & operator=(const Face_Algorith &);//禁止赋值拷贝函数。
public:

    /*! \brief 用于获得SingleTon实例，使用单例模式。
     *  \return SingleTon实例的引用。
     */
    static Face_Algorith* GetInstance()
    {

        if(_face_alg_instance==NULL)//第一次检测
        {
            QMutexLocker locker(&_face_alg_mutex);//加互斥锁。
            _face_alg_instance = new Face_Algorith();//第二次检测。
        }
        return _face_alg_instance;
    }

    //zhu ce qian xu yao she zhi zhu ce yong hu
    void inline SetPersonInfo(int id,QString name,QString departname){
//        std::cout<<"id : "<<id<<" name :"<<name.toStdString()<<std::endl;
        _user.id=id;
        _user.name = name;
        _user.depart_name = departname;
        std::cout<<"id : "<< _user.id<<" name :"<<_user.name.toStdString()<<std::endl;
        _isSetPerson= true;
    }

    void StartWorker();

    void SetIdentifyWork();
    void SetEnrollWork();
    void SetNoWork();
    // void LoginIdentify();
    void UpdateImage(cv::Mat im);

    void CodeCompare(std::vector<float> source);
    bool SaveFeature(std::vector<float> &feat, cv::Mat im, Mat reg_face);
    FaceWorker* GetWork(){
        if(_pfaceWorker ==NULL){
            StartWorker();
        }
        return _pfaceWorker;
    }

    IrisPositionFlag GetTip(int ret);
signals:

    sigFaceState(InteractionResultType,IrisPositionFlag);
    sigIdentSucsses(PersonInfo info);
    sigBoxsChanged(IrisPositionFlag flag,std::vector<float> &face_box, cv::Mat &out_face);
//    sigIdentState(int, std::vector<std::vector<float>>);
public slots:

private:
    int _statue;
    PersonInfo _user;
    QSqlTableModel *_pFaceDataModel;
//    std::vector<std::vector<float>> _vfaceExist;
    bool _isSetPerson;

    FeatureManage  _imagemanager;

    FaceWorker * _pfaceWorker;
    QMutex _save_im_mutex;//实例互斥锁。
    static QMutex _face_alg_mutex;//实例互斥锁。
    static Face_Algorith *_face_alg_instance;/*!<使用原子指针,默认初始化为0。*/
    std::vector<FaceDataTP> _faceDataTPs;



};

#endif // FACE_ALGORITH_H
