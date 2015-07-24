QT += core gui
QT += script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = ../bin
OBJECTS_DIR = ../build
MOC_DIR=../build
TARGET = firetrack
TEMPLATE = app

HEADERS += ftcontroller.h \
    ftelectrodearrayview.h \
    ftelectrodearraywidget.h \
    firetrackwidget.h
SOURCES += firetrackmain.cpp ftcontroller.cpp \
    ftelectrodearrayview.cpp \
    ftelectrodearraywidget.cpp \
    firetrackwidget.cpp

INCLUDEPATH += ../../spikespy/src
DEPENDPATH += ../../spikespy/src
VPATH += ../../spikespy/src

SOURCES += \
    sstimeseriesplot.cpp \
    plotarea.cpp \
    sstimeseriesview.cpp \
    sscontroller.cpp \
    sstimeserieswidget.cpp \
    mdaobject.cpp \
    diskarraymodel.cpp \
    sslabelsmodel1.cpp \
    mdaio.cpp \
    ssabstractview.cpp \
    sslabelview.cpp \
    ssabstractplot.cpp \
    sslabelplot.cpp \
    extractclipsdialog.cpp \
    cvcombowidget.cpp \
    diskreadmda.cpp \
    diskwritemda.cpp \
    memorymda.cpp \
    usagetracking.cpp \
    sscommon.cpp

HEADERS  += sstimeseriesplot.h \
    plotarea.h \
    sstimeseriesview.h \
    sstimeserieswidget.h \
    sscontroller.h \
    mdaobject.h \
    diskarraymodel.h \
    sscommon.h \
    mdaio.h \
    sslabelsmodel.h \
    sslabelsmodel1.h \
    ssabstractview.h \
    sslabelview.h \
    ssabstractplot.h \
    sslabelplot.h \
    extractclipsdialog.h \
    cvcombowidget.h \
    diskreadmda.h \
    diskwritemda.h \
    memorymda.h \
    usagetracking.h

SOURCES += cvwidget.cpp cvview.cpp affinetransformation.cpp cvcommon.cpp
HEADERS += cvwidget.h cvview.h affinetransformation.h cvcommon.h

HEADERS += mda.h textfile.h
SOURCES += mda.cpp textfile.cpp

FORMS += \
    extractclipsdialog.ui

