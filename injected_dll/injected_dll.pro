TARGET = injected_dll
TEMPLATE = lib

QMAKE_CXXFLAGS += -std=c++11

QT += core network
QT -= gui widgets

INCLUDEPATH += ../libqtscreen/directx/

HEADERS += \
    injected_app.h \
    dxscreen.h \
    hook.h

SOURCES += main.cpp \
    injected_app.cpp \
    dx8screen.cpp \
    dx9screen.cpp \
    dx10screen.cpp \
    dx11screen.cpp \
    dxgi_screen.cpp \
    dxscreen.cpp \
    hook.cpp \
    opengl.cpp
