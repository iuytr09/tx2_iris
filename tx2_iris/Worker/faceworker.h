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
        _im = im;
        _isReady = true;

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
    void Run();

private:
    static void * FaceThread(void* arg); //上传图片数据线程
signals:

public slots:

private:
    bool _isruning;
    bool _isReady;
    FaceState _fs;
    std::thread _eid;
    cv::Mat _im;
    QMutex _face_mutex;//实例互斥锁。
    Face_Algorith* am;
};


#endif // FACEWORKER_H
