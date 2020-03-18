#-------------------------------------------------
#
# Project created by QtCreator 2016-03-01T11:11:16
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = latest_test
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    CV_Sources/camera.cpp \
    CV_Sources/color_utility.cpp \
    CV_Sources/face_utility.cpp \
    CV_Sources/num_to_str.cpp \
    CV_Sources/shape_utility.cpp \
    audiorecorder.cpp \
    qaudiolevel.cpp \
    utility_functions.cpp

HEADERS  += mainwindow.h \
    CV_Sources/camera.h \
    CV_Sources/color_utility.h \
    CV_Sources/face_utility.h \
    CV_Sources/num_to_str.h \
    CV_Sources/shape_utility.h \
    audiorecorder.h \
    qaudiolevel.h \
    utility_functions.h

FORMS    += \
    mainwindow.ui \
    audiorecorder.ui \
    audiorecorder_small.ui

INCLUDEPATH += C:\OpenCV\build\MinGW\install\include

LIBS += -LC:\\OpenCV\\build\MinGW\\bin \
    libopencv_core2410 \
    libopencv_highgui2410 \
    libopencv_imgproc2410 \
    libopencv_features2d2410 \
    libopencv_calib3d2410 \
    libopencv_objdetect2410 \
    libopencv_contrib2410 \
    C:\\Windows\\System32\\winmm.dll \
