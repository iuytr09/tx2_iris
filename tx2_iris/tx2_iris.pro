#-------------------------------------------------
# tx2_iris项目创建
# 创建人  ： liu
# 创建时间：2018-10-14T19:13:14
#
#-------------------------------------------------

QT       += core gui multimedia widgets
QT       += sql

TARGET = iris
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialoglogin.cpp \
    dialogabout.cpp \
    enroll.cpp \
    dialogenroll.cpp \
    dialogidentify.cpp \
    manager.cpp \
    suspendbar.cpp \
    syssettings.cpp \
    personmanage.cpp \
    detailpersoninfo.cpp \
    datamanage.cpp \
    shutdown.cpp \
    application.cpp \
    messagebox.cpp \
    messagebox.h \
    departmanage.cpp \
    capturephoto.cpp \
    network.cpp \
    interaction.cpp \
    dbconnection.cpp \
    Common/common.cpp \
    VideoCapture/usbvideocap.cpp \
    FeatureManage.cpp \
    Worker/workerthread.cpp \
    GxIAPI/Fps.cpp \
    VideoCapture/irisvideo.cpp \
    Algorithm/iris_algorith.cpp \
    Worker/enrollworker.cpp \
    Worker/identworker.cpp \
    Algorithm/face_algorith.cpp \
    Worker/faceworker.cpp



HEADERS  += mainwindow.h \
    dialoglogin.h \
    dialogabout.h \
    enroll.h \
    dialogenroll.h \
    dialogidentify.h \
    manager.h \
    suspendbar.h \
    syssettings.h \
    personmanage.h \
    detailpersoninfo.h \
    datamanage.h \
    shutdown.h\
    application.h\
    datamanage.h \
    messagebox.h \
    departmanage.h \
    capturephoto.h \
    #../AlgApi/SRC/AlgApi.h\
    interaction.h \
    network.h\
   # ../bin/serialmuxdemux.h \
    dbconnection.h \
    Common/common.h \
    ../JD_IRIS_API/include/jd_iris.h \
    VideoCapture/usbvideocap.h \
    messagebox.h \
    FeatureManage.h \
    Worker/workerthread.h \
    GxIAPI/Fps.h \
    GxIAPI/GxIAPI.h \
    VideoCapture/irisvideo.h \
    Algorithm/iris_algorith.h \
    Worker/enrollworker.h \
    Worker/identworker.h \
    Algorithm/face_algorith.h \
    ../jdFace_sdk_2.0.0/visi_error.h \
    ../jdFace_sdk_2.0.0/visi_face.h \
    Worker/faceworker.h


    messagebox.h \

FORMS    += mainwindow.ui \
    dialoglogin.ui \
    dialogabout.ui \
    enroll.ui \
    dialogenroll.ui \
    dialogidentify.ui \
    manager.ui \
    suspendbar.ui \
    syssettings.ui \
    personmanage.ui \
    detailpersoninfo.ui \
    datamanage.ui \
    shutdown.ui \
    messagebox.ui \
    departmanage.ui \
    capturephoto.ui
INCLUDEPATH +=/usr/include/opencv2

LIBS += -L/usr/local/lib/ \
    -L/usr/lib/ \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \
    -lgxiapi \
    -liris-sdk \
    -lvisiFace



RESOURCES += \
    setmsgboxbtn2cn.qrc \
    tx2_iris.qrc


#QMAKE_CXXFLAGS += -std=gnu++0x -fpermissive
QMAKE_CXXFLAGS += -std=gnu++11 -fpermissive

OTHER_FILES +=

#QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -std=c++11


