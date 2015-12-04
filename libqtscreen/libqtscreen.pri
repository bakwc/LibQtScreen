QT += core network

HEADERS += $$PWD/inject.h \
    $$PWD/proto.h \
    $$PWD/saveload.h \
    $$PWD/dummy_window.h \
    $$PWD/dxoffsets.h \
    $$PWD/client.h \
    $$PWD/screenshot_maker.h

SOURCES += $$PWD/inject.cpp \
    $$PWD/dummy_window.cpp \
    $$PWD/dx8offsets.cpp \
    $$PWD/dx9offsets.cpp \
    $$PWD/dxgi_offsets.cpp \
    $$PWD/client.cpp \
    $$PWD/screenshot_maker.cpp
