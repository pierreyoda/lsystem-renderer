#-------------------------------------------------
#
# Project created by QtCreator 2014-12-27T14:25:34
#
#-------------------------------------------------

CONFIG += c++11
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LSystemRenderer
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    LSystem.cpp \
    LSystemRendererWidgetBase.cpp \
    LSystemPainterWidget.cpp

HEADERS  += MainWindow.h \
    LSystem.h \
    LSystemRendererWidgetBase.h \
    VirtualTurtle.h \
    LSystemPainterWidget.h

FORMS    += mainwindow.ui
