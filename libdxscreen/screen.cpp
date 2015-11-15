#include "screen.h"
#include "dxoffsets.h"
#include "inject.h"

#include <windows.h>
#include <winuser.h>

#include <QDebug>

TScreenShotMaker::TScreenShotMaker() {
    GetDX8Offsets(InjectorHelpInfo.DX8PresentOffset);
    GetDX9Offsets(InjectorHelpInfo.DX9PresentOffset,
                  InjectorHelpInfo.DX9PresentExOffset);
    GetDXGIOffsets(InjectorHelpInfo.DXGIPresentOffset);

    connect(&Server, &QLocalServer::newConnection, [this] {
        Connections.emplace_back(new TClient(this, Server.nextPendingConnection()));
        TClient* client = Connections.back().get();
        connect(client, &TClient::OnScreenshotReady, [this, client] {
            LastScreenshot = client->GetLastScreenshot();
            emit OnScreenshotReady();
        });
    });
    startTimer(1000);
    Server.listen("mothership");
}

void TScreenShotMaker::MakeScreenshot() {
    for (auto&& cli: Connections) {
        cli->MakeScreenshot();
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
    /// For testing purpose - to inject to signle process only.
    /*
    int pid = GetProcessID(L"dxtest_dx9.exe");
    if (!pid) {
        return;
    }
    static bool inj = false;
    if (inj) {
        return;
    }
    if (!InjectDll(pid, INJECTED_DLL)) {
        return;
    }
    inj = true;
    qDebug() << "injected";
    return;
    */
    HWND window = GetForegroundWindow();
    if (!window) {
        return;
    }
    RECT rect;
    if (!GetWindowRect(window, &rect)) {
        return;
    }

    DWORD styles = GetWindowLongPtr(window, GWL_STYLE);
    if ((styles & WS_MAXIMIZE) && (styles & WS_BORDER)) {
        return;
    }

    MONITORINFO lpmi = {0};
    HMONITOR monitor = MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return;
    }
    lpmi.cbSize = sizeof(lpmi);
    if (!GetMonitorInfoA(monitor, &lpmi)) {
        return;
    }

    if (rect.left != lpmi.rcMonitor.left ||
        rect.right != lpmi.rcMonitor.right ||
        rect.bottom != lpmi.rcMonitor.bottom ||
        rect.top != lpmi.rcMonitor.top)
    {
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(window, &pid);
    if (!pid) {
        return;
    }

    if (InjectedPIDs.find(uint64_t(pid)) != InjectedPIDs.end()) {
        return;
    }

    bool injected = InjectDll(pid, INJECTED_DLL);
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
