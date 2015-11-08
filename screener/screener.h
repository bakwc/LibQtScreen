#pragma once

#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <memory>

#include <libdxscreen/proto.h>

class TScreener;
class TClient: public QObject {
    Q_OBJECT
public:
    TClient(TScreener* screener, QLocalSocket* sock);
    virtual ~TClient();
private:
    void timerEvent(QTimerEvent*);

    bool ProcessBuffer();
    void OnPacketReceived(ECommand cmd, const QByteArray& data);
    void Send(ECommand cmd, const QByteArray& data);
private:
    TScreener* Screener;
    TInjectedAppInfo Info;
    QLocalSocket* Sock;
    QByteArray Buffer;
};
using TClientRef = std::unique_ptr<TClient>;

class TScreener : public QCoreApplication {
    Q_OBJECT
    friend class TClient;
public:
    TScreener(int &argc, char **argv);
    bool Inject();
private:
    QLocalServer Server;
    std::vector<TClientRef> Connections;
    TInjectorHelpInfo InjectorHelpInfo;
};
