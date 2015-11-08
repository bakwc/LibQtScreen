TARGET = injected_dll
TEMPLATE = lib
SOURCES += main.cpp \
    injected_app.cpp
QMAKE_CXXFLAGS += -std=c++11
QT += core network
QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    injected_app.h

INCLUDEPATH += $$PWD/../

win32:CONFIG(release, debug|release): {

    # libinject
    LIBS += -L$$OUT_PWD/../libdxscreen/release/ -llibdxscreen
    PRE_TARGETDEPS += $$OUT_PWD/../libdxscreen/release/liblibdxscreen.a

} else:win32::CONFIG(debug, debug|release): {

    # libinject
    LIBS += -L$$OUT_PWD/../libdxscreen/debug/ -llibdxscreen
    PRE_TARGETDEPS += $$OUT_PWD/../libdxscreen/debug/liblibdxscreen.a

}
