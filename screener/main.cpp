#include <QLocalServer>
#include <QLocalSocket>

#include <screener.h>

#include <functional>
#include <iostream>
#include <QByteArray>

using TCallback = std::function<void(const QByteArray&)>;

struct TMyCtx {
    void* Ptr;
    TCallback Cbk;
};
static TMyCtx* Ctx = new TMyCtx();

void Foo(const TCallback& cbk) {
    Ctx->Cbk = cbk;
}

int main(int argc, char **argv) {

//    Foo([](const QByteArray&){
//        std::cerr << "test\n";
//    });
//    return 42;

    TScreener screener(argc, argv);
    return screener.exec();
}
