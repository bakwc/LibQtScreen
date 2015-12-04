#pragma once

#include "../libqtscreen/proto.h"

#include <QObject>
#include <QLocalSocket>
#include <QByteArray>
#include <windows.h>

class TInjectedApp : public QObject {
    Q_OBJECT
public:
    TInjectedApp();
signals:
    void onScreenshotReadySignal(QByteArray img);
private slots:
    void onScreenshotReady(QByteArray img);
private:
    bool ProcessBuffer();
    void OnPacketReceived(NQtScreen::ECommand cmd, const QByteArray& data);
    void Send(NQtScreen::ECommand cmd, const QByteArray& data);
    HMODULE GetSystemModule(const char* module);
    bool MakeScreenshot();
    void timerEvent(QTimerEvent*);
private:
    NQtScreen::TInjectedAppInfo Info;
    NQtScreen::TInjectorHelpInfo HelpInfo;
    QLocalSocket Sock;
    QByteArray Buffer;
};
