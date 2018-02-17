#-------------------------------------------------
#
# Project created by QtCreator 2018-02-14T21:04:03
#
#-------------------------------------------------

#русская кодировка в выводе приложения
QMAKE_EXTRA_TARGETS += before_build makefilehook

makefilehook.target = $(MAKEFILE)
makefilehook.depends = .beforebuild

PRE_TARGETDEPS += .beforebuild

before_build.target = .beforebuild
before_build.depends = FORCE
before_build.commands = chcp 1251
#

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UdpClient
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        client.cpp \
    registrationusers.cpp \
    authorizationusers.cpp \
    inquirystate.cpp

HEADERS += \
        client.h \
    registrationusers.h \
    authorizationusers.h \
    inquirystate.h

FORMS += \
        client.ui \
    registrationusers.ui \
    authorizationusers.ui

RESOURCES += \
    res.qrc
