TARGET = injected_dll
TEMPLATE = lib
SOURCES += main.cpp \
    injected_app.cpp \
    ../libdxscreen/proto.cpp \
    ../libdxscreen/dx8screen.cpp \
    ../libdxscreen/dx9screen.cpp \
    ../libdxscreen/dx10screen.cpp \
    ../libdxscreen/dx11screen.cpp \
    ../libdxscreen/dxgi_screen.cpp \
    ../libdxscreen/dxscreen.cpp \
    ../libdxscreen/opengl.cpp \
    ../libdxscreen/hook.cpp
QMAKE_CXXFLAGS += -std=c++11
QT += core network

QT -= gui widgets


QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    injected_app.h \
    ../libdxscreen/proto.h \
    ../libdxscreen/dxscreen.h \
    ../libdxscreen/hook.h
