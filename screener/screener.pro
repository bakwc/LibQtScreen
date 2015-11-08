TARGET=screener
TEMPLATE=app

INCLUDEPATH += $$PWD/../

SOURCES += main.cpp \
    screener.cpp

HEADERS += \
    screener.h

QT += core network

QMAKE_CXXFLAGS += -std=c++11

win32:CONFIG(release, debug|release): {

    # libdxscreen
    LIBS += -L$$OUT_PWD/../libdxscreen/release/ -llibdxscreen
    PRE_TARGETDEPS += $$OUT_PWD/../libdxscreen/release/liblibdxscreen.a

    # injected_dll
    DEFINES += INJECTED_DLL=\\\"$$OUT_PWD/../injected_dll/release/injected_dll.dll\\\"

} else:win32::CONFIG(debug, debug|release): {

    # libdxscreen
    LIBS += -L$$OUT_PWD/../libdxscreen/debug/ -llibdxscreen
    PRE_TARGETDEPS += $$OUT_PWD/../libdxscreen/debug/liblibdxscreen.a

    # injected_dll
    DEFINES += INJECTED_DLL=\\\"$$OUT_PWD/../injected_dll/debug/injected_dll.dll\\\"

}
