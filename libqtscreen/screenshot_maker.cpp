#include "screenshot_maker.h"

#include <QFileInfo>
#include <QProcess>
#include <exception>

#include <windows.h>
#include <winuser.h>

#pragma comment(lib, "user32.lib")

namespace NQtScreen {

static bool IsOs64Bit() {
#if defined(_WIN64)
    return true;
#elif defined(_WIN32)
    BOOL res = false;
    IsWow64Process(GetCurrentProcess(), &res);
    return res;
#else
    return false;
#endif
}

static bool IsProcess64Bit(uint32_t pid) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pid);
    BOOL res = false;
    IsWow64Process(processHandle, &res);
    return !res;
}

static QString GetFullPath(const QString& path) {
    QFileInfo checkFile(path);
    return checkFile.absoluteFilePath();
}

static bool FileExists(const QString& path) {
    QFileInfo checkFile(path);
    return checkFile.exists() && checkFile.isFile();
}

static void GetOffsets(TInjectorHelpInfo& offsets, const QString& helperExe) {
    QProcess helper32;
    QStringList args;
    args.push_back("offsets");
    helper32.start(helperExe, args);
    helper32.waitForStarted();
    QByteArray data;
    while (helper32.waitForReadyRead()) {
        data.append(helper32.readAll());
    }
    QList<QByteArray> lines = data.split('\n');
    if (lines.size() < 4) {
        throw std::runtime_error("wrong helper output");
    }
    offsets.DX8PresentOffset = lines[0].trimmed().toInt();
    offsets.DX9PresentOffset = lines[1].trimmed().toInt();
    offsets.DX9PresentExOffset = lines[2].trimmed().toInt();
    offsets.DXGIPresentOffset = lines[3].trimmed().toInt();
}

static bool InjectDll(uint32_t processId, const QString& helperExe, const QString& dll) {
    QProcess helper32;
    QStringList args;
    args.push_back("inject");
    args.push_back(QString::number(processId));
    args.push_back(dll);
    helper32.start(helperExe, args);
    helper32.waitForStarted();
    QByteArray data;
    while (helper32.waitForReadyRead()) {
        data.append(helper32.readAll());
    }
    return helper32.exitCode() == 0;
}

TScreenShotMaker::TScreenShotMaker(const TScreenShotMakerConfig& config)
    : Config(config)
    , MakingScreen(false)
    , FullScreenProcessID(0)
    , ProcessDetectedCounter(0)
    , IsOs64(IsOs64Bit())
{
    if (!FileExists(config.DLL32Path) ||
        !FileExists(config.DLL64Path))
    {
        throw std::runtime_error("libqtscreen32.dll or libqtscreen64.dll not found");
    }

    if (!FileExists(config.Helper32Path) ||
        !FileExists(config.Helper64Path))
    {
        throw std::runtime_error("helper32.exe or helper64.exe not found");
    }

    GetOffsets(InjectorHelpInfo32, config.Helper32Path);
    GetOffsets(InjectorHelpInfo64, config.Helper64Path);

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

    connect(&TimeoutTimer, &QTimer::timeout, [this] {
        TimeoutTimer.stop();
        MakingScreen = false;
        emit OnFailed();
    });

    startTimer(500);
    Server.setSocketOptions(QLocalServer::WorldAccessOption);
    Server.listen("libqtscreen");
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

bool TScreenShotMaker::InjectToPID(uint64_t PID) {
    if (InjectedPIDs.find(PID) != InjectedPIDs.end()) {
        return true;
    }

    bool injected = false;
    if (IsOs64 && IsProcess64Bit(PID)) {
        injected = InjectDll(PID, Config.Helper64Path, GetFullPath(Config.DLL64Path));
    } else {
        injected = InjectDll(PID, Config.Helper32Path, GetFullPath(Config.DLL32Path));
    }
    return injected;
}

void TScreenShotMaker::timerEvent(QTimerEvent*) {
    RemoveInactiveConnections();
    if (Config.AutoInjectToFullscreen) {
        InjectAll();
    }
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

    if (pid != FullScreenProcessID) {
        ProcessDetectedCounter = 0;
    }

    FullScreenProcessID = pid;

    ProcessDetectedCounter += 1;

    //   Give some time to process to initialize
    // and to connect to ourselv if we already injected
    if (ProcessDetectedCounter > 3) {
        InjectToPID(pid);
        // Next inject retry in 30 seconds
        ProcessDetectedCounter = -60;
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
