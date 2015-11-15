#include "client.h"
#include "screen.h"

#include <sstream>
#include <QBuffer>

TClient::TClient(TScreenShotMaker* screener, QLocalSocket* sock)
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

void TClient::MakeScreenshot() {
    Send(CMD_ScreenShot, "doit");
}

QImage TClient::GetLastScreenshot() const {
    return LastScreenshot;
}

const TInjectedAppInfo& TClient::GetInfo() const {
    return Info;
}

bool TClient::IsActive() const {
    return Sock->state() == QLocalSocket::ConnectedState;
}

void TClient::timerEvent(QTimerEvent *) {
    Send(CMD_Ping, "ping");
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
        QByteArray nonConstData = data;
        QBuffer buffer(&nonConstData);
        buffer.open(QIODevice::ReadOnly);
        LastScreenshot.load(&buffer, "png");
        emit OnScreenshotReady();
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
