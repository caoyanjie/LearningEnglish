#-------------------------------------------------
#
# Project created by QtCreator 2015-12-01T11:29:32
#
#-------------------------------------------------

QT       += core gui
QT       += xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LearningEglish
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp\
        xmlprocess.cpp\
        windowtools.cpp

HEADERS  += widget.h\
        xmlprocess.h\
        windowtools.h

RC_ICONS = English.ico

RESOURCES += \
    pix.qrc
