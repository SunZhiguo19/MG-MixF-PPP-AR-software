#-------------------------------------------------
#
# Project created by QtCreator 2021-08-20T14:25:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MG-MixF PPP-AR
TEMPLATE = app

include($$PWD/rinex_dat/rinex_dat.pri)
include($$PWD/pos_alg/pos_alg.pri)
include($$PWD/filter_alg/filter_alg.pri)
include($$PWD/ppp_model/ppp_model.pri)
include($$PWD/autoresize/autoresize.pri)
include($$PWD/inf_write/inf_write.pri)
SOURCES += main.cpp\
        heu_agnss.cpp \
    SetWidget.cpp \
    ConfigWidget.cpp \
    ConfTranIni.cpp \
    AGNSS_APPData.cpp \
    skyview.cpp \
    app.cpp \
    widget.cpp

INCLUDEPATH +=\
$$PWD/resources/myeigen

HEADERS  += heu_agnss.h\
            pos_algorith.h \
    ConfigWidget.h \
    SetWidget.h \
    ConfTranIni.h \
    AGNSS_APPData.h \
    skyview.h \
    app.h \
    myhelper.h \
    widget.h


FORMS    += heu_agnss.ui \
    SetWidget.ui \
    ConfigWidget.ui \
    widget.ui


QT       += charts

RESOURCES += \
    resources.qrc
