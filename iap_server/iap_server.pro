#-------------------------------------------------
#
# Project created by QtCreator 2017-05-10T15:56:59
#
#-------------------------------------------------

QT       += core gui
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iap_server
TEMPLATE = app


SOURCES += main.cpp\
    iap_server.cpp \
    mytcpserver.cpp \
    mytcpclient.cpp \
    iap.cpp \
    crc.cpp

HEADERS  += iap_server.h \
    mytcpserver.h \
    mytcpclient.h \
    myhelper.h \
    iap_server.h \
    iap.h \
    crc.h

FORMS    += iap_server.ui

OTHER_FILES += \
   update.rc \
   update.ico
RC_FILE = update.rc
