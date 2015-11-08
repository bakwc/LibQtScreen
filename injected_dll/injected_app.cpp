#include "injected_app.h"

#include <libdxscreen/dxscreen.h>

#include <QProcess>
#include <QDebug>
#include <sstream>
#include <QBuffer>

TInjectedApp::TInjectedApp(int& argc, char **argv)
    : QCoreApplication(argc, argv)
{
    connect(&Sock, &QLocalSocket::readyRead, [this] {
        Buffer += Sock.readAll();
        while (ProcessBuffer()) {
        }
    });

    // Required to pass screenshot from directX thread to our thread.
    connect(this, &TInjectedApp::onScreenshotReadySignal,
            this, &TInjectedApp::onScreenshotReady,
            Qt::BlockingQueuedConnection);

    Info.PID = applicationPid();
    Info.Name = applicationFilePath().toStdString();
    Info.HasDirectX = false;

    Sock.connectToServer("mothership");
}

void TInjectedApp::onScreenshotReady(QImage img) {
    QByteArray packet;
    QBuffer buffer(&packet);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "png");
    Send(CMD_ScreenShot, packet);
}

bool TInjectedApp::ProcessBuffer() {
    if (Buffer.size() < 2) {
        return false;
    }
    uint16_t packetSize = *(uint16_t*)Buffer.data();
    // todo: check packet size
    if (Buffer.size() < packetSize + 2) {
        return false;
    }
    uint16_t packetType = *(uint16_t*)(Buffer.data() + 2);
    QByteArray packetData = QByteArray(Buffer.data() + 4, packetSize - 2);
    Buffer.remove(0, packetSize + 2);
    OnPacketReceived((ECommand)packetType, packetData);
    return true;
}

void TInjectedApp::OnPacketReceived(ECommand cmd, const QByteArray& data) {
    switch (cmd) {
    case CMD_Ping: {
        Send(CMD_Ping, "pong");
    } break;
    case CMD_Info: {
        imemstream in(data.data(), data.size());
        HelpInfo.Load(in);
        std::stringstream out;
        Info.Save(out);
        Send(CMD_Info, QByteArray::fromStdString(out.str()));
    } break;
    case CMD_ScreenShot: {
        MakeScreenshot();
    } break;
    default:
        break;
    }
}

void TInjectedApp::Send(ECommand cmd, const QByteArray& data) {
    QByteArray packet;
    packet.resize(4);
    packet += data;
    *(uint16_t*)(packet.data() + 2) = (uint16_t)cmd;
    *(uint16_t*)(packet.data()) = (uint16_t)(data.size() + 2);
    Sock.write(packet);
}

void TInjectedApp::MakeScreenshot() {
    MakeDX8Screen([this](const QImage& img) {
        emit onScreenshotReadySignal(img);
    }, HelpInfo.DX8PresentOffset);
}
