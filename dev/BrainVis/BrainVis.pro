#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T11:10:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BrainVis
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    renderwidget.cpp \
    openglwidget.cpp \
    informationwidget.cpp \
    datahandler.cpp \
    metawidget.cpp \
    planingwidget.cpp \
    drivewidget.cpp \
    ctregistrationwidget.cpp \
    histogrammwidget.cpp

HEADERS  += mainwindow.h \
    renderwidget.h \
    openglwidget.h \
    informationwidget.h \
    datahandler.h \
    metawidget.h \
    planingwidget.h \
    drivewidget.h \
    ctregistrationwidget.h \
    histogrammwidget.h

FORMS    += mainwindow.ui \
    renderwidget.ui \
    informationwidget.ui \
    metawidget.ui \
    planingwidget.ui \
    drivewidget.ui \
    ctregistrationwidget.ui \
    histogrammwidget.ui
