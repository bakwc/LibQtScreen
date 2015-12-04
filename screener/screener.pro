TARGET=screener
TEMPLATE=app

INCLUDEPATH += $$PWD/../

SOURCES += main.cpp \
    screener.cpp

HEADERS += \
    screener.h

QT += gui widgets core network

QMAKE_CXXFLAGS += -std=c++11

include(../libqtscreen/libqtscreen.pri)
