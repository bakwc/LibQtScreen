TARGET=libdxscreen
TEMPLATE=lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

HEADERS += inject.h \
    hook.h \
    proto.h \
    saveload.h \
    dummy_window.h \
    dxoffsets.h \
    dxscreen.h
SOURCES += inject.cpp \
    hook.cpp \
    proto.cpp \
    saveload.cpp \
    dummy_window.cpp \
    dx8offsets.cpp \
    dxscreen.cpp \
    dx8screen.cpp
