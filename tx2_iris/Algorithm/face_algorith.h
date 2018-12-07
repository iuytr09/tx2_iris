#ifndef FACE_ALGORITH_H
#define FACE_ALGORITH_H

#include <QObject>
#include<QMutexLocker>
#include<QSqlTableModel>
#include "Common/common.h"
#include "../jdFace_sdk_2.0.0/visi_face.h"

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


    void Identify(cv::Mat im);
    void LoginIdentify(cv::Mat im);
    void Enroll(cv::Mat im);
private:
    bool saveFeature(std::vector<float> &feat,cv::Mat im);
    void codeCompare(std::vector<float> source);
signals:
    sigEnrollSuccess(int);

public slots:

private:
    int _statue;
    PersonInfo _user;
    QSqlTableModel *_pFaceDataModel;
     std::vector<std::vector<float>> _vfaceExist;
    bool _isSetPerson;

    QMutex _save_im_mutex;//实例互斥锁。
    static QMutex _face_alg_mutex;//实例互斥锁。
    static Face_Algorith *_face_alg_instance;/*!<使用原子指针,默认初始化为0。*/


//    QVector<EnrollWorker*> _vEnrollworkers;
//    int _eWorkSize;
//    int _curEIndex;
//    bool _isEnrollStop;

//    QVector<IdentWorker*> _vIdentWorkers;
//    int _iWorkSize;
//    int _curIIndex;
//    bool _isIdentStop;
};

#endif // FACE_ALGORITH_H
