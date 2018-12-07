#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include<QThread>
#include<QThreadPool>


enum RunType{
    UNKNOWN = 0,
    Enroll =1,
    Ident=2,
    LoginIdent=3
};

class WorkerThread : public QThread
{
    Q_OBJECT

    explicit WorkerThread(QObject *parent = 0);
//    void run() Q_DECL_OVERRIDE {
//        QString result;
//        /* ... here is the expensive or blocking operation ... */
//        emit resultReady(result);
//    }

    inline void SetType(RunType t){
        _type=t;
    }

    inline RunType GetType(){
       return _type;
    }

    inline  void SetStop(){
        _isruning=false;
    }

    inline bool GetRuning(){
       return _isruning;
    }

signals:
    void resultReady(const QString &s);

private:
    bool _isruning;
    RunType _type;
};

//void MyObject::startWorkInAThread()
//{
//    WorkerThread *workerThread = new WorkerThread(this);
//    connect(workerThread, &WorkerThread::resultReady, this, &MyObject::handleResults);
//    connect(workerThread, &WorkerThread::finished, workerThread, &QObject::deleteLater);
//    workerThread->start();
//}

#endif // WORKERTHREAD_H
