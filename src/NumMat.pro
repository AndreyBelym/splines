#-------------------------------------------------
#
# Project created by QtCreator 2012-09-22T11:49:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += qwt
INCLUDEPATH += F:/Qt/Qwt-6.0.1/include

INCLUDEPATH += C:/Users/Wolf/Documents/NumMat/muparser/include
LIBS += C:/Users/Wolf/Documents/NumMat/muparser/lib/libmuparser.a

TARGET = NumMat
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SplinesInterpol.cpp

HEADERS  += mainwindow.h \
    SplinesInterpol.h

FORMS    += mainwindow.ui

OTHER_FILES +=

RESOURCES += \
    NumMat.qrc
