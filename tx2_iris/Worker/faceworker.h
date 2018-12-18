#ifndef FACEWORKER_H
#define FACEWORKER_H

#include <QObject>

#include <QObject>
#include<QVector>
#include<opencv.hpp>
#include <QMutexLocker>
#include<thread>


class Face_Algorith;

enum  FaceState{
    FaceUnknown=0,
    FaceIdent=1,
    FaceEnroll=2
};

class FaceWorker : public QObject
{
    Q_OBJECT
public:
    explicit FaceWorker(QObject *parent = 0);
    ~FaceWorker();

    void  Push(cv::Mat im){
        QMutexLocker locker(&_face_mutex);
        _ims.clear();
        _ims.push_back(im);
    }

    cv::Mat GetIM(){
        QMutexLocker locker(&_face_mutex);
        cv::Mat im= _ims.front();
        _ims.pop_front();
        return  im;
    }

  void ClearIM(){
        QMutexLocker locker(&_face_mutex);
        _ims.clear();
    }

  inline void SetState(FaceState s){
      _fs = s;
  }

    inline  void SetStop(){
        _isruning=false;
    }

    inline bool GetRuning(){
        return _isruning;
    }

private:
    static void * FaceThread(void* arg); //上传图片数据线程
signals:

public slots:

private:
    bool _isruning;
    FaceState _fs;
    std::thread _eid;
    QVector<cv::Mat> _ims;
    QMutex _face_mutex;//实例互斥锁。
    Face_Algorith* am;
};


#endif // FACEWORKER_H
