#pragma once

#include <libdxscreen/proto.h>
#include <QObject>
#include <QLocalSocket>
#include <QImage>
#include <windows.h>

class TInjectedApp : public QObject {
    Q_OBJECT
public:
    TInjectedApp();
signals:
    void onScreenshotReadySignal(QImage img);
private slots:
    void onScreenshotReady(QImage img);
private:
    bool ProcessBuffer();
    void OnPacketReceived(ECommand cmd, const QByteArray& data);
    void Send(ECommand cmd, const QByteArray& data);
    HMODULE GetSystemModule(const char* module);
    void MakeScreenshot();
private:
    TInjectedAppInfo Info;
    TInjectorHelpInfo HelpInfo;
    QLocalSocket Sock;
    QByteArray Buffer;
};
