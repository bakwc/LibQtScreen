#include "screenshot_maker.h"
#include "dxoffsets.h"
#include "inject.h"

#include <QFileInfo>

#include <windows.h>
#include <winuser.h>

namespace NQtScreen {

static bool FileExists(const QString& path) {
    QFileInfo checkFile(path);
    return checkFile.exists() && checkFile.isFile();
}

TScreenShotMaker::TScreenShotMaker(const TScreenShotMakerConfig& config)
    : Config(config)
    , MakingScreen(false)
    , FullScreenProcessID(0)
{
    Q_ASSERT(FileExists(config.DLL32Path) && "dll does not exist");
    GetDX8Offsets(InjectorHelpInfo.DX8PresentOffset);
    GetDX9Offsets(InjectorHelpInfo.DX9PresentOffset,
                  InjectorHelpInfo.DX9PresentExOffset);
    GetDXGIOffsets(InjectorHelpInfo.DXGIPresentOffset);

    connect(&Server, &QLocalServer::newConnection, [this] {
        Connections.emplace_back(new TClient(this, Server.nextPendingConnection()));
        TClient* client = Connections.back().get();
        connect(client, &TClient::OnScreenshotReady, [this, client] {
            if (!MakingScreen) {
                return;
            }
            LastScreenshot = client->GetLastScreenshot();
            MakingScreen = false;
            TimeoutTimer.stop();
            emit OnScreenshotReady();
        });
        connect(client, &TClient::OnFailed, [this] {
            CheckFailedScreens();
        });
    });

    connect(&TimeoutTimer, QTimer::timeout, [this] {
        TimeoutTimer.stop();
        MakingScreen = false;
        emit OnFailed();
    });

    startTimer(500);
    Server.listen("mothership");
}

void TScreenShotMaker::MakeScreenshot() {
    if (Connections.size() == 0 || FullScreenProcessID == 0) {
        emit OnFailed();
        return;
    }
    for (auto&& cli: Connections) {
        if (cli->GetInfo().PID == FullScreenProcessID) {
            MakingScreen = true;
            TimeoutTimer.start(1000);
            cli->MakeScreenshot();
            break;
        }
    }
}

QImage TScreenShotMaker::GetLastScreenshot() {
    return LastScreenshot;
}

void TScreenShotMaker::timerEvent(QTimerEvent*) {
    RemoveInactiveConnections();
    InjectAll();
}

void TScreenShotMaker::InjectAll() {
    HWND window = GetForegroundWindow();
    if (!window) {
        FullScreenProcessID = 0;
        return;
    }
    RECT rect;
    if (!GetWindowRect(window, &rect)) {
        FullScreenProcessID = 0;
        return;
    }

    DWORD styles = GetWindowLongPtr(window, GWL_STYLE);
    if ((styles & WS_MAXIMIZE) && (styles & WS_BORDER)) {
        FullScreenProcessID = 0;
        return;
    }

    MONITORINFO lpmi = {0};
    HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        FullScreenProcessID = 0;
        return;
    }
    lpmi.cbSize = sizeof(lpmi);
    if (!GetMonitorInfoA(monitor, &lpmi)) {
        FullScreenProcessID = 0;
        return;
    }

    if (rect.left != lpmi.rcMonitor.left ||
        rect.right != lpmi.rcMonitor.right ||
        rect.bottom != lpmi.rcMonitor.bottom ||
        rect.top != lpmi.rcMonitor.top)
    {
        FullScreenProcessID = 0;
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (!pid) {
        FullScreenProcessID = 0;
        return;
    }

    FullScreenProcessID = pid;

    if (InjectedPIDs.find(uint64_t(pid)) != InjectedPIDs.end()) {
        return;
    }

    bool injected = InjectDll(pid, Config.DLL32Path.toStdString());
    if (!injected) {
        return;
    }
}

void TScreenShotMaker::RemoveInactiveConnections() {
    std::vector<TClientRef> newConnections;
    InjectedPIDs.clear();
    for (auto&& c: Connections) {
        if (c->IsActive()) {
            newConnections.push_back(c);
            if (c->GetInfo().PID) {
                InjectedPIDs.insert(uint64_t(c->GetInfo().PID));
            }
        }
    }
    Connections.swap(newConnections);
}

void TScreenShotMaker::CheckFailedScreens() {
    if (!MakingScreen) {
        return;
    }
    bool anyInProgress = false;
    for (auto&& c: Connections) {
        if (!c->IsScreenFailed()) {
            anyInProgress = true;
        }
    }
    if (!anyInProgress) {
        MakingScreen = false;
        TimeoutTimer.stop();
        emit OnFailed();
    }
}

} // NQtScreen
