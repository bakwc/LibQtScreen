#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QImage>
#include <memory>

#include "proto.h"

namespace NQtScreen {

class TScreenShotMaker;
class TClient: public QObject {
    Q_OBJECT
public:
    TClient(TScreenShotMaker* screener, QLocalSocket* sock);
    virtual ~TClient();
    void MakeScreenshot();
    QImage GetLastScreenshot() const;
    const TInjectedAppInfo& GetInfo() const;
    bool IsActive() const;
    bool IsScreenFailed() const;
signals:
    void OnScreenshotReady();
    void OnFailed();
private:
    void timerEvent(QTimerEvent*);

    bool ProcessBuffer();
    void OnPacketReceived(ECommand cmd, const QByteArray& data);
    void Send(ECommand cmd, const QByteArray& data);
private:
    TScreenShotMaker* Screener;
    TInjectedAppInfo Info;
    QLocalSocket* Sock;
    QByteArray Buffer;
    QImage LastScreenshot;
    bool Failed;
};
using TClientRef = std::shared_ptr<TClient>;

} // NQtScreen
