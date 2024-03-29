#-------------------------------------------------
#
# Project created by QtCreator 2018-05-19T04:45:37
#
#-------------------------------------------------

# IMPORTANT:    Opensuse leap 15: Install kio-devel first
QT       += core gui widgets KIOCore KIOFileWidgets KIOWidgets KNTLM

#   For some reason qDebug didn't display anything in application output panel
#   using console as an alternative
#   CONFIG += console

TARGET = Rename
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui

CONFIG += c++11
