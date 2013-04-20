#-------------------------------------------------
#
# Project created by QtCreator 2012-12-29T09:07:39
#
#-------------------------------------------------

QT       += core gui network

TARGET = GutenbergGui
TEMPLATE = app


SOURCES += src/main.cpp \
        src/mainwindow.cpp

HEADERS  += src/mainwindow.h

FORMS    += src/mainwindow.ui
LIBS += -lwinspool
