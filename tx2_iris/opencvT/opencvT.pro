QT += core
QT -= gui

TARGET = opencvT
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

INCLUDEPATH +=   /usr/include \
                 /usr/include/opencv2
LIBS += -L/usr/lib/ \
    -lopencv_core \
    -lopencv_highgui \
    -lopencv_imgproc \

