/*****************************************************************************
** 文 件 名： main.h
** 主 要 类： 主程序
**
** Copyright (c) shanggushijue
**
** 创 建 人：刘中昌
** 创建时间：2018-10-24
**
** 修 改 人：
** 修改时间：
** 描  述:    项目入口
** 主要模块说明: 项目入口
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <QApplication>
#include <QTranslator>
#include "mainwindow.h"
#include <QTextCodec>
#include<QSqlTableModel>
#include<QSqlRecord>
#include<iostream>
#include<QSqlError>
#include<QMessageBox>
#include<QSqlQuery>



#include "interaction.h"

//#include "application.h"

#include "dbconnection.h"

void test(){
    QSqlQuery query;
//    query.prepare("INSERT INTO TEST(ID,NAME) VALUES (?,?)");
//    query.addBindValue(1);
//     query.addBindValue("_user.name");
//     query.exec();

    query.prepare("INSERT INTO facedata(id,uid,name,depart_name,feat) VALUES (NULL,:uid,:name,:depart_name,:feat)");

    query.bindValue(":uid", 1);
    query.bindValue(":name", "_user.name");
    query.bindValue(":depart_name", "_user.depart_name");

    QByteArray array;

//    //int l=sizeof(feat); // 4*4 = 16 (一个float占4个字节)
//    int len_fVar =  4*feat.size();//
//    array.resize(len_fVar);
//    memcpy(array.data(), &feat[0], len_fVar);
    array.append("sdklakflsdafkaldkflsdakflad");

    query.bindValue(":feat",  array);
    query.exec();

    if(!query.isActive())
    {
        QMessageBox::warning(NULL,"数据库存储人脸信息失败",query.lastError().text());
    }else{
        std::cout<<"存储虹膜信息成功!!"<<std::endl;
    }
}

int main(int argc, char *argv[])
{
    QTranslator oTranslator;
    oTranslator.load(":/qt_zh_CN");
    QApplication a(argc, argv);
    a.setStyle("macintosh");
    a.installTranslator(&oTranslator);   //汉化消息框按钮

    Interaction::GetInteractionInstance()->PlayInteractionSound(InteractionResultType::IrisEnrollSuccess,IrisPositionFlag::Far);

//    if(!createConnection(NULL))      //创建数据库连接
//    {
//        return 0;
//    }
//    test();

//   //ce shi shu ju ku cao zuo
//    QSqlTableModel *_pIrisDataModel = new QSqlTableModel(NULL);//QSqlTableModel
//    _pIrisDataModel->setTable("irisdata");
//    _pIrisDataModel->select();

//    //ceshi
//    for(int i=0;i<_pIrisDataModel->rowCount();i++){
//        QSqlRecord record= _pIrisDataModel->record(i);
//        std::cout<<"id:"<<record.value("id").toInt()<<"uid:"<<record.value("uid").toInt()<<"irisdata:"<<record.value("datatemple").toString().toStdString()<<std::endl;
//    }

    //增加中文的显示
//    QCoreApplication::addLibraryPath("./plugins");
//    QCoreApplication::addLibraryPath("./qt.plugins");
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));
//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("System"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));

    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
  //  QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
  //  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    //QTextCodec::setCodecForTr(QTextCodec::codecForName("GB18030"));
    MainWindow w;
    w.show();

    return a.exec();
}

//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>

//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/core/core.hpp>

//using namespace cv;
//int main( int argc, char** argv )
//{
////    Mat image;
////    image = imread( "/root/work/虹膜软件/tx2_iris/image/back1.jpg" );

////    namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
////    imshow( "Display Image", image );
////    waitKey(0);

////    return 0;

//    VideoCapture cap(0);
//       if(!cap.isOpened())
//       {
//           return -1;
//       }
//       Mat frame;
//       Mat edges;

//       bool stop = false;
//       while (!stop)
//           {
//               cap >> frame;
//               imshow("读取视频", frame);
//               if(waitKey(30) >=0)
//                   stop = true;
//           }

////       while(!stop)
////       {
////           cap>>frame;
////           cvtColor(frame, edges, CV_BGR2GRAY);
////           GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
////           Canny(edges, edges, 0, 30, 3);
////           imshow("当前视频",edges);
////           if(waitKey(30) >=0)
////               stop = true;
////       }
//       return 0;
//}
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//using namespace cv;
//int main(int argc, char *argv[])
//{
//    namedWindow( "src", WINDOW_AUTOSIZE );
//    Mat src1 = imread( "/root/work/虹膜软件/tx2_iris/image/09.ico" );
//    while(1) {
//        imshow( "src", src1 );
//        char c = waitKey(30);
//        if( 27==c )
//            return 0;
//    }
//}
