/*****************************************************************************
** 文 件 名：IRIS_Algorith.cpp
** 主 要 类：IRIS_Algorith
**
** Copyright (c) shanggushijue
**
** 创 建 人：liuzhch
** 创建时间：2018-11-06
**
** 修 改 人：
** 修改时间：
** 描  述:
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "iris_algorith.h"
#include <QSqlRecord>
#include <stdio.h>
#include<qdebug.h>
#include <omp.h>
#include<QSqlQuery>
#include<QMessageBox>
#include<QSqlError>
#include<string.h>
////#include<stdio.h>
///
#include "dbconnection.h"
//静态成员变量初始化。
QMutex IRIS_Algorith::_alg_mutex;
IRIS_Algorith* IRIS_Algorith::_alg_instance = NULL;



/*****************************************************************************
*                        函数
*  函 数 名：IRIS_Algorith
*  功    能：虹膜算法初始化
*  说    明：构造函数
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
IRIS_Algorith::IRIS_Algorith():_isOK(false),_isSetPerson(false)
{
    if(createConnection(NULL))      //创建数据库连接
    {

        st_JD_IRIS_ACINFO acInfo;
        _pJD_IRIS = JD_IRIS::JD_IRIS_Create(acInfo);

        if(0==acInfo.nIsACOK)
        {
            //0,授权失败;
            _err = "授权失败!";
        }else{
            //1,授权成功
            stINITSTRUCT initPara;
            initPara.emCodeFunChoice = IRIS_CODE_METHOD1;
            initPara.nCPUGPU = 0;
            strncpy(initPara.strParaDir, "/home/nvidia/work/JD_IRIS_API/para", GBMaxFileNameLen);
            strncpy(initPara.strModelDir, "/home/nvidia/work/JD_IRIS_API/models-enc", GBMaxFileNameLen);

            JD_IRIS_STATUS nStatus = _pJD_IRIS->JD_IRIS_Init(initPara);

            if (JD_STATUS_OK==nStatus){
                _err="成功!";
                _isOK = true;
                // shu ju shou xian jia zai dao nei cun
                _pIrisDataModel = new QSqlTableModel();
                _pIrisDataModel->setTable("irisdata");
                _pIrisDataModel->select();

                //chu shi hua tu xiang
                _imagemanager.InitParam();

                //ce shi shu ju ku cao zuo
                for(int i=0;i<_pIrisDataModel->rowCount();i++){
                    QSqlRecord record= _pIrisDataModel->record(i);
                    std::cout<<"id:"<<record.value("id").toInt()<<"uid:"<<record.value("uid").toInt()<<"irisdata:"<<record.value("datatemple").toString().toStdString()<<std::endl;
                }

            }else{
                _err= getError(nStatus);
            }
        }
    }else{
        _err = "创建数据库连接失败!";
    }
}

/*****************************************************************************
*                        函数
*  函 数 名：~IRIS_Algorith
*  功    能：析构函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
IRIS_Algorith::~IRIS_Algorith()
{
    if(_isOK){
        _pJD_IRIS->JD_IRIS_Free();
    }
    if(_pIrisDataModel!=NULL)
    {
        delete _pIrisDataModel;
        _pIrisDataModel = NULL;
    }
    this->Release();
}

/*****************************************************************************
*                        函数
*  函 数 名：getError
*  功    能：根据状态返回错误信息
*  说    明：
*  参    数：错误码
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString IRIS_Algorith::getError(int status){
    QString err="成功!";
    switch (status) {
    case JD_STATUS_OK:
        err="成功!";
        break;
    case JD_STATUS_PARAMETER_INVALID:
        err="参数无效!";
        break;
    case JD_STATUS_PARAMETER_OUTOF_BOUND:
        err="参数越界!";
        break;
    case JD_STATUS_OUTOF_MEMORY:
        err="没有足够的系统内存!";
        break;
    case JD_STATUS_NO_EYE_FOUND:
        err="没有找到眼睛!";
        break;
    case JD_STATUS_PUPIL_OUTOF_BOUND:
        err="瞳孔超出图像外!";
        break;
    case JD_STATUS_IRIS_OUTOF_BOUND:
        err="虹膜超出图像外!";
        break;
    case JD_STATUS_SEG_FAIL:
        err="分割失败分割失败!";
        break;
    case JD_STATUS_IRIS_AREA_NOT_ENOUGH:
        err="虹膜区域不足!";
        break;
    default:
        err="其他问题!";
        break;
    }
    qDebug()<<err;
    return err;
}


/**
 * @brief IRIS_Algorith::setIndentCount
 * @param size
 */
void IRIS_Algorith::setIndentCount(int size){

    if(_isOK){
        if(_vIdentWorkers.size()>0){
            int count = _vIdentWorkers.size();
            for(int i=0;i<count;i++){
                IdentWorker *e = _vIdentWorkers.front();
                _vIdentWorkers.pop_front();
                delete e;
                e=NULL;
            }
        }

        _iWorkSize = size;
        _curIIndex =0;
        for(int i=0;i<size;i++){
            IdentWorker * e = new IdentWorker();

            _vIdentWorkers.push_back(e);
        }
    }

}

/*****************************************************************************
*                        函数
*  函 数 名：setEnrollCount
*  功    能：设置注册线程个数
*  说    明：
*  参    数：int size
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::setEnrollCount(int size){
    if(_isOK){
        if(_vEnrollworkers.size()>0){
            int count = _vEnrollworkers.size();
            for(int i=0;i<count;i++){
                EnrollWorker *e = _vEnrollworkers.front();
                _vEnrollworkers.pop_front();
                delete e;
                e=NULL;
            }
        }

        _eWorkSize = size;
        _curEIndex =0;
        for(int i=0;i<size;i++){
            EnrollWorker * e = new EnrollWorker();
            _vEnrollworkers.push_back(e);
        }
    }
}

//设置聚合个数5取3（5） 或 9取5（9）
const int nTemplatesNum =5;
/*****************************************************************************
*                        函数
*  函 数 名：SaveFeature
*  功    能：注册虹膜保存虹膜图像和虹膜特征
*  说    明：该图像左右眼特征都被提取。
*  参    数：l左眼特征 r 右眼特征 im虹膜图像
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IRIS_Algorith::SaveFeature(JD_IRIS_TEMPLATE *l, JD_IRIS_TEMPLATE *r, IRIS_Mat_Quality im){


    QMutexLocker locker(&_save_im_mutex);//加互斥锁。
    JD_IRIS_TEMPLATE *tl = new JD_IRIS_TEMPLATE();
    tl->nLength=l->nLength;
    tl->pData = new char[tl->nLength];
    memcpy(tl->pData,l->pData,tl->nLength);
    _pleftT.push_back(tl);


    JD_IRIS_TEMPLATE *jd_tr = new JD_IRIS_TEMPLATE();
    jd_tr->nLength=r->nLength;
    jd_tr->pData = new char[jd_tr->nLength];
    memcpy(jd_tr->pData,r->pData,jd_tr->nLength);
    _prightT.push_back(jd_tr);

    _images.push_back(im);

    if(_pleftT.size()==nTemplatesNum && _prightT.size()==nTemplatesNum ){

        stop_Enroll();
        //ting zhi zu ce

        int* pIndex1 = new int[nTemplatesNum];
        int* pIndex2 = new int[nTemplatesNum];


        JD_IRIS_STATUS nStatus1 = JD_STATUS_OK;
        JD_IRIS_STATUS nStatus2 = JD_STATUS_OK;

#pragma omp parallel sections
        {
#pragma omp section
            {
                nStatus1 = _pJD_IRIS->JD_IRIS_TemplateChoice(_pleftT,3,pIndex1);
            }
#pragma omp section
            {
                nStatus2 = _pJD_IRIS->JD_IRIS_TemplateChoice(_prightT,3,pIndex2);
            }
        }

        if(nStatus1==JD_STATUS_OK&& nStatus2==JD_STATUS_OK){
            QSqlQuery query;
            query.prepare("INSERT INTO irisdata(uid,name,depart_name,l_1_tc,l_2_tc,l_3_tc,r_1_tc,r_2_tc,r_3_tc) VALUES(:uid,:name,:depart_name,:l_1_tc,:l_2_tc,:l_3_tc,:r_1_tc,:r_2_tc,:r_3_tc)");

            query.bindValue(":uid", _user.id);
            query.bindValue(":name", _user.name);
            query.bindValue(":depart_name", _user.depart_name);
            int li=1,ri=1;
            for(int index=0;index<nTemplatesNum;index++){
                if(pIndex1[index]==1&&li<=3){
                    QString tb = QString(":l_%1_tc").arg(li);
                    QByteArray str(_pleftT.at(li)->pData,_pleftT.at(li)->nLength);
                    query.bindValue(tb,  str);
                    li++;
                }

                if(pIndex2[index]==1&& ri<=3){
                    QString tb = QString(":r_%1_tc").arg(ri);
                    //std::string str = (char*)_pleftT.at(ri)->pData;
                    QByteArray str(_prightT.at(ri)->pData,_prightT.at(ri)->nLength);
                    query.bindValue(tb,  str);
                    ri++;
                }
                _imagemanager.SaveImage(_user.id,index,_images.at(index).im);
            }
            query.exec();

            if(!query.isActive())
            {
                QMessageBox::warning(NULL,tr("数据库存储虹膜信息失败"),query.lastError().text());
            }else{
                std::cout<<"存储虹膜信息成功!!"<<std::endl;
            }

            emit sigIrisState(InteractionResultType::IrisEnrollSuccess,IrisPositionFlag::Unknown);

            //zu ce cheng gong

        }else{
            //zu ce shi bai ;
            emit sigIrisState(InteractionResultType::IrisEnrollFailed,IrisPositionFlag::Unknown);
        }


        _vEnrollworkers.at(_curEIndex)->ClearIM();
        //Release();

        // v = pIndex[], v==1,选 中的 模 板索 引 ; v==0, 未 选 中的 模 板索 引
        delete [] pIndex1;
        pIndex1 = NULL;
        delete [] pIndex2;
        pIndex2= NULL;
    }

}

/*****************************************************************************
*                        函数
*  函 数 名：Release
*  功    能：释放注册特征及图像资源
*  说    明：在保存特征后需要释放资源
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::Release(){
    QMutexLocker locker(&_save_im_mutex);//加互斥锁。
    int i=0;
    for(i=0;i<_pleftT.size();i++){
        JD_IRIS_TEMPLATE* pTempCode = _pleftT.at(i);
        //资源释放
        if(NULL != pTempCode->pData)
        {
            delete [] pTempCode->pData;
            pTempCode->pData = NULL;
        }
        //        delete &pTempCode;
        //        pTempCode = NULL;
    }
    _pleftT.clear();

    for(i=0;i<_prightT.size();i++){
        JD_IRIS_TEMPLATE* pTempCode = _prightT.at(i);
        //资源释放
        if(NULL != pTempCode->pData)
        {
            delete [] pTempCode->pData;
            pTempCode->pData = NULL;
        }
        //        delete pTempCode;
        //        pTempCode = NULL;
    }
    _prightT.clear();

    _images.clear();
}

/*****************************************************************************
*                        函数
*  函 数 名：start_Enroll()
*  功    能：开始虹膜注册
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::start_Enroll(){
    Release();
    _isEnrollStop = false;
    _isIdentStop =true;
}

/*****************************************************************************
*                        函数
*  函 数 名：stop_Enroll()
*  功    能：停止虹膜注册
*  说    明：1、注册成功或注册失败后都要调用该接口；2、主动调用中间停止注册
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::stop_Enroll(){
    _isEnrollStop =true;
}



/*****************************************************************************
*                        函数
*  函 数 名：release_Enroll()
*  功    能：主动取消注册
*  说    明：中间取消注册需要释放资源
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::release_Enroll(){
    _isEnrollStop = false;
    Release();
}

/*****************************************************************************
*                        函数
*  函 数 名：start_Ident()
*  功    能：启动识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::start_Ident(){
    _isIdentStop = false;
    release_Enroll();
}
/*****************************************************************************
*                        函数
*  函 数 名：stop_Ident()
*  功    能：取消识别
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::stop_Ident(){
    _isIdentStop =true;
}


/*****************************************************************************
*                        函数
*  函 数 名：Enroll
*  功    能：注册函数
*  说    明：将图像按规则放入不同的线程中进行处理
*  参    数：im  虹膜图像
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::Enroll(cv::Mat im){
    if(_isOK ){

        if(!_isSetPerson){

            //qing shou xian she zhi zu ce yong hu!
            //qDebug()<<"请首先设置注册用户，再进行虹膜注册!";
            return;
        }

        if(_isEnrollStop){
           // qDebug()<<"注册已取消!";
            return;
        }
        _vEnrollworkers.at(_curEIndex)->Push(im);//.clone());
        _curEIndex=(_curEIndex+1)% _curEIndex<_vEnrollworkers.size();

    }else{
        //hong mo suan fa cun zai wen ti
        qDebug()<<"[Enroll]虹膜算法存在问题，请确认!";
    }

}


//hui zu ju xing kuang
//C++: void rectangle(Mat& img, Point pt1,Point pt2,const Scalar& color, int thickness=1, int lineType=8, int shift=0)
//C++: void rectangle(Mat& img, Rect rec, const Scalar& color, int thickness=1, int lineType=8, int shift=0 )

/*****************************************************************************
*                        函数
*  函 数 名：Identify
*  功    能：识别函数
*  说    明：将图像按规则放入不同的线程中进行处理
*  参    数：im 虹膜图像
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::Identify(cv::Mat im){

    if(_isOK){
        //声明存放定位结果的变量
        if(_isIdentStop){
           // qDebug()<<"停止识别!";
            return;
        }

        if(_vIdentWorkers.size()>0&& _curIIndex<_vIdentWorkers.size()){
            _vIdentWorkers.at(_curIIndex)->Push(im);
            _curIIndex=(_curIIndex+1)% _curIIndex<_vIdentWorkers.size();
        }

    }else{
        //hong mo suan fa cun zai wen ti
        qDebug()<<"[Identify]虹膜算法存在问题";
    }
}



/*****************************************************************************
*                        函数
*  函 数 名：LoginIdentify
*  功    能：虹膜登录
*  说    明：将图像按规则放入不同的线程中进行处理
*  参    数：im 虹膜图像
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::LoginIdentify(cv::Mat im){

}



/*****************************************************************************
*                        函数
*  函 数 名：codeCompareL
*  功    能：左眼特征比较
*  说    明：
*  参    数：source 带比较特征
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::codeCompareL(JD_IRIS_TEMPLATE* source){
    double dDist=1.0;
    QSqlRecord record;
    for(int i=0;i<_pIrisDataModel->rowCount();i++){
        double dtemp=1.0;
        QSqlRecord recordtemp= _pIrisDataModel->record(i);
        // std::string datatemple=record.value("l_1_tc").toString().toStdString();
        JD_IRIS_TEMPLATE* codeExist = new JD_IRIS_TEMPLATE();
        QByteArray datatemple=recordtemp.value("l_1_tc").toByteArray();
        codeExist->nLength = datatemple.length();

        codeExist->pData = datatemple.data();
        _pJD_IRIS->JD_IRIS_CodeCompare(codeExist,source,dtemp);
        if(dtemp<dDist)
        {
            dDist = dtemp;
            record = recordtemp;
        }
    }

    if(dDist<0.35){
        //匹配成功
        //record  匹配结果
        _recogUser.id = record.value("uid").toInt();
        _recogUser.name = record.value("name").toString();
        _recogUser.depart_name = record.value("depart_name").toString();
        //        emit sigIdentifySuccess(PromptFlag::IRIS_SUCCESS);
        emit  sigIdentSucsses(_recogUser);



    }else{
        //        emit sigIdentifySuccess(PromptFlag::IRIS_FAIL);
        emit sigIrisState(InteractionResultType::IrisIdenFailed,IrisPositionFlag::Unknown);
    }


}

/*****************************************************************************
*                        函数
*  函 数 名：codeCompareR
*  功    能：右眼特征比较
*  说    明：
*  参    数：source 带比较特征
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-11-06
*  修 改 人：
*  修改时间：
*****************************************************************************/
void IRIS_Algorith::codeCompareR(JD_IRIS_TEMPLATE* source){

    double dDist=1.0;
    QSqlRecord record;
    for(int i=0;i<_pIrisDataModel->rowCount();i++){
        double dtemp=1.0;
        QSqlRecord recordtemp= _pIrisDataModel->record(i);
        QByteArray datatemple=recordtemp.value("r_1_tc").toByteArray();
        JD_IRIS_TEMPLATE* codeExist = new JD_IRIS_TEMPLATE();
        codeExist->nLength = datatemple.length();
        codeExist->pData = datatemple.data();
        _pJD_IRIS->JD_IRIS_CodeCompare(codeExist,source,dtemp);
        if(dtemp<dDist)
        {
            dDist = dtemp;
            record = recordtemp;
        }
    }

    if(dDist<0.32){
        //匹配成功
        //record  匹配结果
        _recogUser.id = record.value("uid").toInt();
        _recogUser.name = record.value("name").toString();
        _recogUser.depart_name = record.value("depart_name").toString();
        emit sigIdentSucsses(_recogUser);

    }else{
        emit sigIrisState(InteractionResultType::IrisIdenFailed,IrisPositionFlag::Unknown);
    }
}

///*****************************************************************************
//*                        函数
//*  函 数 名：SendIdentState
//*  功    能：发送识别状态
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：liuzhch
//*  创建时间：2018-12-05
//*  修 改 人：slotBoxsChanged
//*  修改时间：
//*****************************************************************************/
//void IRIS_Algorith::SendIdentState(int state){
//    //emit sigIdentifySuccess(state);
//}

///*****************************************************************************
//*                        函数
//*  函 数 名：SendEnrollState
//*  功    能：发送注册状态
//*  说    明：
//*  参    数：
//*  返 回 值：
//*  创 建 人：liuzhchslotBoxsChanged
//*  创建时间：2018-12-05
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//void IRIS_Algorith::SendEnrollState(int state){
//    emit sigEnrollSuccess(state);
//}

/*****************************************************************************
*                        函数
*  函 数 名： GetTip
*  功    能： 根据位置判断提示
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：liuzhch
*  创建时间：2018-12-25
*  修 改 人：
*  修改时间：
*****************************************************************************/
IrisPositionFlag IRIS_Algorith::GetTip(cv::Rect ret){
    int far = 160;//小 远
    int near = 240;//大  近
    int left =50;
    int right = 3000;
    int up = 50;
    int down = 2000;

    if(ret.x<left){
        return IrisPositionFlag::Left;
    }else{
        return IrisPositionFlag::Right;
    }

    if(ret.width<far){
        return IrisPositionFlag::Far;
    }else if (ret.width>near){
        return IrisPositionFlag::Near;
    }else{
        if(ret.x<left){
            return IrisPositionFlag::Left;
        }else if (ret.x>right){
            return IrisPositionFlag::Right;
        }else{
            if(ret.y<up){
                return IrisPositionFlag::Up;
            }else if (ret.y>down){
                return IrisPositionFlag::Down;
            }else{
                return IrisPositionFlag::OK;
            }
        }
    }

}
