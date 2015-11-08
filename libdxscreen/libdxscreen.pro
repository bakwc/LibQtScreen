TARGET=libdxscreen
TEMPLATE=lib
CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

QT += core network

HEADERS += inject.h \
    hook.h \
    proto.h \
    saveload.h \
    dummy_window.h \
    dxoffsets.h \
    dxscreen.h \
    screen.h \
    client.h

SOURCES += inject.cpp \
    hook.cpp \
    proto.cpp \
    saveload.cpp \
    dummy_window.cpp \
    dx8offsets.cpp \
    dxscreen.cpp \
    dx8screen.cpp \
    screen.cpp \
    client.cpp

# injected_dll
win32:CONFIG(release, debug|release): {
    DEFINES += INJECTED_DLL=\\\"$$OUT_PWD/../injected_dll/release/injected_dll.dll\\\"
} else:win32::CONFIG(debug, debug|release): {
    DEFINES += INJECTED_DLL=\\\"$$OUT_PWD/../injected_dll/debug/injected_dll.dll\\\"
}
