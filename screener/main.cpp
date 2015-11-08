#include <libdxscreen/inject.h>

#include <iostream>
#include <windows.h>

#include <QLocalServer>
#include <QLocalSocket>

#include <screener.h>

int main(int argc, char **argv) {
    TScreener screener(argc, argv);
    screener.Inject();
    return screener.exec();
}
