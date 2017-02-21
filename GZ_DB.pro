#-------------------------------------------------
#
# Project created by QtCreator 2016-02-18T13:56:23
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GZ_DB
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbcontrl.cpp \
    Camera6467.cpp \
    CameraResult.cpp \
    tinyxml/tinystr.cpp \
    tinyxml/tinyxml.cpp \
    tinyxml/tinyxmlerror.cpp \
    tinyxml/tinyxmlparser.cpp \
    customresultlist.cpp \
    thread_db_local.cpp \
    thread_db_remote.cpp \
    thread_db_savestatus.cpp \
    thread_connect_camera.cpp \
    thread_MoveResultToDBList.cpp \
    thread_readcacheresult.cpp


HEADERS  += mainwindow.h \
    dbcontrl.h \
    Camera6467.h \
    CameraResult.h \
    tinyxml/tinystr.h \
    tinyxml/tinyxml.h \
    HvDevice/HvDeviceNew.h \
    customresultlist.h \
    thread_db_local.h \
    thread_db_remote.h \
    thread_db_savestatus.h \
    thread_connect_camera.h \
    thread_MoveResultToDBList.h \
    thread_readcacheresult.h


FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/HvDevice/ -lHvDevice
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/HvDevice/ -lHvDeviced

INCLUDEPATH += $$PWD/HvDevice
DEPENDPATH += $$PWD/HvDevice
