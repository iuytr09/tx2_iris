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
        _user.id=id;
        _user.name = name;
        _user.depart_name = departname;
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
private:


//    //人机交互结果类型
//    enum InteractionResultType
//    {
//        IrisEnrollSuccess,  //注册成功
//        IrisEnrollFailed,   //注册失败
//        IrisEnrollPerformig,//注册中
//        IrisIdenSuccess,    //识别成功
//        IrisIdenFailed,     //识别失败
//        IrisIdenKeepLastStatus,//保存识别上一次结果状态，只界面显示，不播放语音
//        IrisIdenPerforming, //识别中
//        FaceEnrollSuccess,  //注册成功
//        FaceEnrollFailed,   //注册失败
//        FaceEnrollPerformig,//注册中
//        FaceIdenSuccess,    //识别成功
//        FaceIdenFailed,     //识别失败
//        FaceIdenKeepLastStatus,//保存识别上一次结果状态，只界面显示，不播放语音
//        FaceIdenPerforming, //识别中
//        ResultUnknown   //结果未知或者目前没有结果
//    };
//    //位置信息标志
//    enum IrisPositionFlag
//    {
//        Far				=	0,	//位置偏远
//        Near			=	1,  //位置偏近
//        Left			=	2,  //位置偏左
//        Right			=	3,  //位置偏右
//        Up				=	4,  //位置偏上
//        Down			=	5,  //位置偏下
//        OK				=	6,  //位置合适
//        Unknown         =   7   //位置未知，不用提示
//    };
signals:

    sigFaceState(InteractionResultType,IrisPositionFlag);
    sigIdentSucsses(PersonInfo info);
//    sigEnrollState(int state,std::vector<float> &face_box, cv::Mat &out_face);
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
