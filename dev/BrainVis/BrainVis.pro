#-------------------------------------------------
#
# Project created by QtCreator 2015-10-10T11:10:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BrainVis
TEMPLATE = app

RC_FILE = brainvis.rc

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
    histogrammwidget.cpp \
    framewidget.cpp \
    mertool.cpp \
    merwidgets/merimageentry.cpp \
    merwidgets/merelectrodeentry.cpp \
    merwidgets/meroptions.cpp

HEADERS  += mainwindow.h \
    renderwidget.h \
    openglwidget.h \
    informationwidget.h \
    datahandler.h \
    metawidget.h \
    planingwidget.h \
    drivewidget.h \
    ctregistrationwidget.h \
    histogrammwidget.h \
    framewidget.h \
    mertool.h \
    merwidgets/merimageentry.h \
    merwidgets/merelectrodeentry.h \
    merwidgets/meroptions.h

FORMS    += mainwindow.ui \
    renderwidget.ui \
    informationwidget.ui \
    metawidget.ui \
    planingwidget.ui \
    drivewidget.ui \
    ctregistrationwidget.ui \
    histogrammwidget.ui \
    framewidget.ui \
    mertool.ui \
    merwidgets/merimageentry.ui \
    merwidgets/merelectrodeentry.ui \
    merwidgets/meroptions.ui
