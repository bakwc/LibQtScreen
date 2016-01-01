#include <QLocalServer>
#include <QLocalSocket>

#include <screener.h>

#include <functional>
#include <iostream>
#include <QByteArray>

using TCallback = std::function<void(const QByteArray&)>;

int main(int argc, char **argv) {
    TScreener screener(argc, argv);
    return screener.exec();
}
