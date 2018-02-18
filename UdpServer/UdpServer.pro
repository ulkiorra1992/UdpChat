#-------------------------------------------------
#
# Project created by QtCreator 2018-02-14T21:03:13
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

TARGET = UdpServer
TEMPLATE = app

DEFINES += CORE_LIBRARY

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        server.cpp \
    projectsettings.cpp \
    usersettings.cpp

HEADERS += \
        server.h \
    projectsettings.h \
    core_global.h \
    projectnamehelper.h \
    usersettings.h

FORMS += \
        server.ui

RESOURCES += \
    res.qrc
