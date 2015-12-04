#include "client.h"
#include "screenshot_maker.h"

#include <sstream>
#include <QBuffer>
#include <QImage>
#include <QByteArray>

namespace NQtScreen {

QImage RawDataToQImage(EImgByteFormat fmt,
                        const char* intarray,
                        unsigned rows,
                        unsigned columns)
{
    QImage resImg(columns, rows, QImage::Format_RGB32);
    if (fmt == BF_B8G8R8A8 || fmt == BF_R8G8B8A8) {
        for (unsigned row = rows; row; row--) {
            for (unsigned col = 0; col < columns; col++) {
                char red, green, blue;
                int idx = (int(row) - 1) * columns * 4 + col * 4;
                if (fmt == BF_R8G8B8A8) {
                    red = intarray[idx];
                    green = intarray[idx + 1];
                    blue = intarray[idx + 2];
                } else {
                    blue = intarray[idx];
                    green = intarray[idx + 1];
                    red = intarray[idx + 2];
                }
                resImg.setPixel(col, row - 1, qRgb(red, green, blue));
            }
        }
    } else {
        for (size_t j = 0; j < rows; j++) {
            for (size_t i = 0; i < columns; i++) {
                int offset = (i + j * columns) * (3 * sizeof(uint8_t));

                uint8_t r = intarray[offset + 0];
                uint8_t g = intarray[offset + 1];
                uint8_t b = intarray[offset + 2];

                resImg.setPixel(i, j, qRgb(r, g, b));
            }
        }
    }
    return resImg;
}

TClient::TClient(TScreenShotMaker* screener, QLocalSocket* sock)
    : Screener(screener)
    , Sock(sock)
    , Failed(true)
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
    Failed = false;
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

bool TClient::IsScreenFailed() const {
    return Failed;
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
    if ((uint32_t)Buffer.size() < packetSize + 6) {
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
        EImgByteFormat fmt = (EImgByteFormat)data[0];
        uint32_t rows = *(uint32_t*)(data.data() + 1);
        uint32_t columns = *(uint32_t*)(data.data() + 1 + 4);
        LastScreenshot = RawDataToQImage(fmt, data.data() + 1 + 4 + 4, rows, columns);
        emit OnScreenshotReady();
    } break;
    case CMD_Error: {
        Failed = true;
        emit OnFailed();
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

} // NQtScreen
