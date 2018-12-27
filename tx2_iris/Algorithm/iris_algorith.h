/*****************************************************************************
*                        文件
*  文 件 名： iris_algorith.h
*  功    能：
*  说    明：
*  作    者：liuzhch
*  创建时间：2018-12-05
*****************************************************************************/
#ifndef ALGORITHM_H
#define ALGORITHM_H
#include "../JD_IRIS_API/include/jd_iris.h"
#include <opencv2/opencv.hpp>
#include <QString>
#include<QSqlTableModel>
#include "Common/common.h"
#include "FeatureManage.h"
#include<QMutexLocker>
//#include<QAtomicPointer>
#include<QReadWriteLock>
#include"Worker/enrollworker.h"
#include"Worker/identworker.h"
#include<QVector>




//编码信息结构体
typedef struct IRIS_Mat_Quality
{
    cv::Mat im;  //tu xiang
    double leftQuality;
    double rightQuality;

}st_IRIS_Mat_Quality;


////位置信息标志
//enum PromptFlag
//{
//    IRIS_FAIL       =-1,//shibai
//    IRIS_SUCCESS    = 0, //chenggong
//    IRIS_Pos_Far			=	1,	//位置偏远
//    IRIS_Pos_Near			=	2,  //位置偏近
//    IRIS_Pos_Left			=	3,  //位置偏左
//    IRIS_Pos_Right			=	4,  //位置偏右
//    IRIS_Pos_Up				=	5,  //位置偏上
//    IRIS_Pos_Down			=	6,  //位置偏下
//    IRIS_Pos_OK				=	7  //位置合适
//    //Unknown         =   8  //位置未知，不用提示
//};

/*****************************************************************************
*                        类
*  类    名：IRIS_Algorith
*  功    能：算法类
*  说    明：虹膜注册、虹膜识别
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
class IRIS_Algorith :public QObject
{
    Q_OBJECT
private:
    IRIS_Algorith();//禁止构造函数。
    ~IRIS_Algorith();
    IRIS_Algorith(const IRIS_Algorith &);//禁止拷贝构造函数。
    IRIS_Algorith & operator=(const IRIS_Algorith &);//禁止赋值拷贝函数。
public:

    /*! \brief 用于获得SingleTon实例，使用单例模式。
     *  \return SingleTon实例的引用。
     */
    static IRIS_Algorith* GetInstance()
    {

        if(_alg_instance==NULL)//第一次检测
        {
            QMutexLocker locker(&_alg_mutex);//加互斥锁。
            _alg_instance = new IRIS_Algorith();//第二次检测。
        }
        return _alg_instance;
    }

    bool GetStatus(){
        return _isOK;
    }

    QString getError(){
        return _err;
    }

    void Identify(cv::Mat im);
    void LoginIdentify(cv::Mat im);
    void Enroll(cv::Mat im);

    //zhu ce qian xu yao she zhi zhu ce yong hu
    void inline SetPersonInfo(int id,QString name,QString departname){
        _user.id=id;
        _user.name = name;
        _user.depart_name = departname;
        _isSetPerson= true;
    }

    inline int GetLeftSize(){
        return _pleftT.size();
    }
    inline int GetRightsize(){
        return _prightT.size();
    }

    inline int GetMinSize(){
        return _pleftT.size()<_prightT.size()?_pleftT.size():_prightT.size();
    }
    void Release();

    inline std::vector<IRIS_Mat_Quality> GetImages(){
        return _images;
    }

    JD_IRIS* GetJD_IRIS(){
        return _pJD_IRIS;
    }

    QSqlTableModel* GetIrisDataModel(){
        return _pIrisDataModel;
    }

    void setIndentCount(int size);
    void setEnrollCount(int size);
    bool SaveFeature(JD_IRIS_TEMPLATE* l,JD_IRIS_TEMPLATE* r,IRIS_Mat_Quality im);
    void codeCompareL(JD_IRIS_TEMPLATE* source);
    void codeCompareR(JD_IRIS_TEMPLATE* source);
    //void SendIdentState(int state);
    void SendEnrollState(int state);
   IrisPositionFlag GetTip(cv::Rect ret);
public slots:

    void start_Enroll();
    void stop_Enroll();
    void release_Enroll();

    void start_Ident();
    void stop_Ident();

private:
    QString getError(int status);

    bool SaveDBFeature();

signals:

    sigIrisState(InteractionResultType,IrisPositionFlag);
    sigIdentSucsses(PersonInfo p);

    //信号
//    void sigEnrollSuccess(int p);
//    void sigIdentifySuccess(int state);
//    void sigIdentifyLoginSuccess(PersonInfo p);

//    void sigStatus(int type,QString msg);//type 1 红色 ;2 绿色  ; msg 提示信息

private:
    JD_IRIS* _pJD_IRIS;
    bool   _isOK;
    QString _err;
    PersonInfo _user;
    bool _isSetPerson;
    std::vector<JD_IRIS_TEMPLATE*> _pleftT;
    std::vector<JD_IRIS_TEMPLATE*> _prightT;
    std::vector<IRIS_Mat_Quality> _images;

    QSqlTableModel *_pIrisDataModel;
    FeatureManage  _imagemanager;
    PersonInfo   _recogUser;

    //QReadWriteLock internalMutex;//函数使用的读写锁。

    QMutex _save_im_mutex;//实例互斥锁。
    static QMutex _alg_mutex;//实例互斥锁。
    static IRIS_Algorith *_alg_instance;/*!<使用原子指针,默认初始化为0。*/

    QVector<EnrollWorker*> _vEnrollworkers;
    int _eWorkSize;
    int _curEIndex;
    bool _isEnrollStop;

    QVector<IdentWorker*> _vIdentWorkers;
    int _iWorkSize;
    int _curIIndex;
    bool _isIdentStop;
};

//typedef struct  JD_Iris_Result{
//    std::vector<JD_IRIS_TEMPLATE*> _pleftT;
//    std::vector<JD_IRIS_TEMPLATE*> _prightT;
//    std::vector<IRIS_Mat_Quality> _images;
//};


#endif // ALGORITHM_H
