TARGET=helper
TEMPLATE=app

QT += gui widgets core network

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += ../directx_api/

HEADERS += \
    dummy_window.h \
    dxoffsets.h \
    inject.h

SOURCES += main.cpp \
    dummy_window.cpp \
    dx8offsets.cpp \
    dx9offsets.cpp \
    dxgi_offsets.cpp \
    inject.cpp
