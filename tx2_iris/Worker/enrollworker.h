#ifndef ENROLLWORKER_H
#define ENROLLWORKER_H

#include <QObject>
#include<QVector>
#include<opencv.hpp>
#include <QMutexLocker>
#include<thread>



class IRIS_Algorith;
class JD_IRIS;
class EnrollWorker : public QObject
{
    Q_OBJECT
public:
    explicit EnrollWorker(QObject *parent = 0);
    ~EnrollWorker();
    
    void  Push(cv::Mat im){
        QMutexLocker locker(&_enroll_mutex);
        _ims.clear();
        _ims.push_back(im);
    }
    
    cv::Mat GetIM(){
        QMutexLocker locker(&_enroll_mutex);
        cv::Mat im= _ims.front();
        _ims.pop_front();
        return  im;
    }
    
  void ClearIM(){
        QMutexLocker locker(&_enroll_mutex);
        _ims.clear();
    }
    
    inline  void SetStop(){
        _isruning=false;
    }
    
    inline bool GetRuning(){
        return _isruning;
    }
    
private:
    static void * EnrolThread(void* arg); //上传图片数据线程
signals:
    
public slots:
    
private:
    bool _isruning;
    std::thread _eid;
    QVector<cv::Mat> _ims;
    QMutex _enroll_mutex;//实例互斥锁。
    IRIS_Algorith* am;
    JD_IRIS* pJD_IRIS;
};

#endif // ENROLLWORKER_H
