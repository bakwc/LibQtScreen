#include "screen.h"
#include "dxoffsets.h"
#include "inject.h"

#include <windows.h>
#include <psapi.h>

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

    DWORD procList[1024];
    DWORD cbNeeded;
    EnumProcesses(procList, sizeof(procList), &cbNeeded);
    size_t procCount = cbNeeded / sizeof(DWORD);

    for (size_t i = 0; i < procCount; ++i) {
        DWORD pid = procList[i];

        if (InjectedPIDs.find(uint64_t(pid)) != InjectedPIDs.end()) {
            continue;
        }

        bool injected = InjectDll(pid, INJECTED_DLL);
        if (!injected) {
            continue;
        }

        InjectedPIDs.insert(uint64_t(pid));
    }
}
