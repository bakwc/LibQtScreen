TARGET=screener
TEMPLATE=app

INCLUDEPATH += $$PWD/../

SOURCES += main.cpp \
    screener.cpp

HEADERS += \
    screener.h

QT += core network

QMAKE_CXXFLAGS += -std=c++11

# libdxscreen
win32:CONFIG(release, debug|release): {
    LIBS += -L$$OUT_PWD/../libdxscreen/release/ -llibdxscreen -lpsapi
    PRE_TARGETDEPS += $$OUT_PWD/../libdxscreen/release/liblibdxscreen.a
} else:win32::CONFIG(debug, debug|release): {
    LIBS += -L$$OUT_PWD/../libdxscreen/debug/ -llibdxscreen -lpsapi
    PRE_TARGETDEPS += $$OUT_PWD/../libdxscreen/debug/liblibdxscreen.a
}
