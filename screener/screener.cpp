#include "screener.h"

#include <libdxscreen/inject.h>
#include <libdxscreen/dxoffsets.h>
#include <iostream>
#include <sstream>
#include <QDebug>
#include <QBuffer>
#include <QImage>

TClient::TClient(TScreener* screener, QLocalSocket* sock)
    : Screener(screener)
    , Sock(sock)
{
    connect(sock, &QLocalSocket::readyRead, [this] {
        Buffer += Sock->readAll();
        while (ProcessBuffer()) {
        }
    });
    std::stringstream out;
    Screener->InjectorHelpInfo.Save(out);
    Send(CMD_Info, QByteArray::fromStdString(out.str()));
    startTimer(1000);
}

TClient::~TClient() {
}

void TClient::timerEvent(QTimerEvent *) {
    Send(CMD_Ping, "ping");
    static int n = 0;
    n += 1;
    if (n == 5) {
        qDebug() << "requesting screenshot";
        Send(CMD_ScreenShot, "doit");
    }
}

bool TClient::ProcessBuffer() {
    if (Buffer.size() < 2) {
        return false;
    }
    uint32_t packetSize = *(uint32_t*)Buffer.data();
    // todo: check packet size
    if (Buffer.size() < packetSize + 6) {
        return false;
    }
    uint16_t packetType = *(uint16_t*)(Buffer.data() + 4);
    QByteArray packetData = QByteArray(Buffer.data() + 6, packetSize);
    Buffer.remove(0, packetSize + 6);
    OnPacketReceived((ECommand)packetType, packetData);
    return true;
}

void TClient::OnPacketReceived(ECommand cmd, const QByteArray& data) {
    switch (cmd) {
    case CMD_Ping: {
    } break;
    case CMD_Info: {
        imemstream in(data.data(), data.size());
        Info.Load(in);
    } break;
    case CMD_ScreenShot: {
        QImage img;
        QByteArray nonConstData = data;
        QBuffer buffer(&nonConstData);
        buffer.open(QIODevice::ReadOnly);
        img.load(&buffer, "png");
        img.save("C:\\koding\\screener_dump.png");
    } break;
    }
}

void TClient::Send(ECommand cmd, const QByteArray& data) {
    QByteArray packet;
    packet.resize(6);
    packet += data;
    *(uint32_t*)(packet.data()) = (uint32_t)(data.size());
    *(uint16_t*)(packet.data() + 4) = (uint16_t)cmd;
    Sock->write(packet);
}

TScreener::TScreener(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    GetDX8Offsets(InjectorHelpInfo.DX8PresentOffset);
    connect(&Server, &QLocalServer::newConnection, [this] {
        Connections.emplace_back(new TClient(this, Server.nextPendingConnection()));
    });
    startTimer(1000);
    Server.listen("mothership");
}

bool TScreener::Inject() {
    //int pid = GetProcessID(L"dxtest_dx8.exe");
    int pid = GetProcessID(L"BF1942.exe");
    if (!pid) {
        qDebug() << "inject process not found";
        return false;
    }

    bool injected = InjectDll(pid, INJECTED_DLL);
    if (!injected) {
        qDebug() << "failed to inject";
        return false;
    }

    qDebug() << "injected";
    return true;
}

void TScreener::timerEvent(QTimerEvent *) {
    static int n = 0;
    n += 1;
    if (n == 10) {
        Inject();
    }
}
