#include "injected_app.h"
#include "dxscreen.h"

#include <QProcess>
#include <sstream>
#include <QBuffer>
#include <QCoreApplication>

TInjectedApp::TInjectedApp() {
    connect(&Sock, &QLocalSocket::readyRead, [this] {
        Buffer += Sock.readAll();
        while (ProcessBuffer()) {
        }
    });

    // Required to pass screenshot from directX thread to our thread.
    connect(this, &TInjectedApp::onScreenshotReadySignal,
            this, &TInjectedApp::onScreenshotReady,
            Qt::BlockingQueuedConnection);

    Info.PID = QCoreApplication::instance()->applicationPid();
    Info.Name = QCoreApplication::instance()->applicationFilePath().toStdString();

    startTimer(500);
}

void TInjectedApp::onScreenshotReady(QByteArray img) {
    Send(NQtScreen::CMD_ScreenShot, img);
}

bool TInjectedApp::ProcessBuffer() {
    if (Buffer.size() < 4) {
        return false;
    }
    uint32_t packetSize = *(uint32_t*)Buffer.data();
    // todo: check packet size
    if ((uint32_t)Buffer.size() < packetSize + 6u) {
        return false;
    }
    uint16_t packetType = *(uint16_t*)(Buffer.data() + 4);
    QByteArray packetData = QByteArray(Buffer.data() + 6, packetSize);
    Buffer.remove(0, packetSize + 6);
    OnPacketReceived((NQtScreen::ECommand)packetType, packetData);
    return true;
}

void TInjectedApp::OnPacketReceived(NQtScreen::ECommand cmd, const QByteArray& data) {
    switch (cmd) {
    case NQtScreen::CMD_Ping: {
        Send(NQtScreen::CMD_Ping, "pong");
    } break;
    case NQtScreen::CMD_Info: {
        NQtScreen::imemstream in(data.data(), data.size());
        HelpInfo.Load(in);
        std::stringstream out;
        Info.Save(out);
        Send(NQtScreen::CMD_Info, QByteArray::fromStdString(out.str()));
    } break;
    case NQtScreen::CMD_ScreenShot: {
        if (!MakeScreenshot()) {
            Send(NQtScreen::CMD_Error, "error");
        }
    } break;
    default:
        break;
    }
}

void TInjectedApp::Send(NQtScreen::ECommand cmd, const QByteArray& data) {
    QByteArray packet;
    packet.resize(6);
    packet += data;
    *(uint32_t*)(packet.data()) = (uint32_t)(data.size());
    *(uint16_t*)(packet.data() + 4) = (uint16_t)cmd;
    Sock.write(packet);
}

bool TInjectedApp::MakeScreenshot() {
    bool success = false;
    success |= MakeDX8Screen([this](const QByteArray& img) {
        emit onScreenshotReadySignal(img);
    }, HelpInfo.DX8PresentOffset);
    success |= MakeDX9Screen([this](const QByteArray& img) {
        emit onScreenshotReadySignal(img);
    }, HelpInfo.DX9PresentOffset, HelpInfo.DX9PresentExOffset);
    success |= MakeDXGIScreen([this](const QByteArray& img) {
        emit onScreenshotReadySignal(img);
    }, HelpInfo.DXGIPresentOffset);
    success |= MakeOpenGLScreen([this](const QByteArray& img) {
        emit onScreenshotReadySignal(img);
    });
    return success;
}

void TInjectedApp::timerEvent(QTimerEvent *) {
    if (Sock.state() == QLocalSocket::UnconnectedState) {
        Sock.connectToServer("libqtscreen");
    }
}
