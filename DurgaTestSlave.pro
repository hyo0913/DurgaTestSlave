#-------------------------------------------------
#
# Project created by QtCreator 2019-11-19T11:51:31
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DurgaTestSlave
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    HyoFrame.cpp \
    HyoDioForm.cpp \
    HyoDurgaModuleForm.cpp

HEADERS  += mainwindow.h \
    HyoFrame.h \
    HyoDioForm.h \
    HyoDurgaModuleForm.h \
    DurgaTestCommon.h

FORMS    += mainwindow.ui \
    HyoDioForm.ui \
    HyoDurgaModuleForm.ui

RESOURCES += \
    DurgaTestResource.qrc
