#include "screenshot_maker.h"

#include <QFileInfo>
#include <QProcess>
#include <QDate>
#include <QTime>

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

bool TScreenShotMaker::GetOffsets(TInjectorHelpInfo& offsets, const QString& helperExe) {
    QProcess helper32;
    QStringList args;
    args.push_back("offsets");
    helper32.start(helperExe, args);
    if (!helper32.waitForStarted()) {
        WriteLog(LL_Error, "failed to execute helper " + helperExe);
        return false;
    }
    QByteArray data;
    while (helper32.waitForReadyRead()) {
        data.append(helper32.readAll());
    }
    QList<QByteArray> lines = data.split('\n');
    if (lines.size() < 4) {
        WriteLog(LL_Error, "wrong output for helper " + helperExe);
        return false;
    }
    offsets.DX8PresentOffset = lines[0].trimmed().toInt();
    offsets.DX9PresentOffset = lines[1].trimmed().toInt();
    offsets.DX9PresentExOffset = lines[2].trimmed().toInt();
    offsets.DXGIPresentOffset = lines[3].trimmed().toInt();
    WriteLog(LL_Info, "offsets for " + helperExe);
    WriteLog(LL_Info, QString("DX8PresentOffset: %1").arg(offsets.DX8PresentOffset));
    WriteLog(LL_Info, QString("DX9PresentOffset: %1").arg(offsets.DX9PresentOffset));
    WriteLog(LL_Info, QString("DX9PresentExOffset: %1").arg(offsets.DX9PresentExOffset));
    WriteLog(LL_Info, QString("DXGIPresentOffset: %1").arg(offsets.DXGIPresentOffset));
    return true;
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
    Initialized = false;
    WriteLog(LL_Info, "Starting LibQtScreen");
    if (!FileExists(config.DLL32Path) ||
        !FileExists(config.DLL64Path))
    {
        WriteLog(LL_Error, "libqtscreen32.dll or libqtscreen64.dll not found");
        return;
    }

    if (!FileExists(config.Helper32Path) ||
        !FileExists(config.Helper64Path))
    {
        WriteLog(LL_Error, "helper32.exe or helper64.exe not found");
        return;
    }

    if (!GetOffsets(InjectorHelpInfo32, config.Helper32Path)) {
        return;
    }

    if (!GetOffsets(InjectorHelpInfo64, config.Helper64Path)) {
        return;
    }

    connect(&Server, &QLocalServer::newConnection, [this] {
        WriteLog(LL_Info, "new connection");
        Connections.emplace_back(new TClient(this, Server.nextPendingConnection()));
        TClient* client = Connections.back().get();
        connect(client, &TClient::OnScreenshotReady, [this, client] {
            if (!MakingScreen) {
                WriteLog(LL_Warning, "screenshot received after timeout");
                return;
            }
            LastScreenshot = client->GetLastScreenshot();
            MakingScreen = false;
            TimeoutTimer.stop();
            WriteLog(LL_Info, "screenshot successful");
            emit OnScreenshotReady();
        });
        connect(client, &TClient::OnFailed, [this] {
            CheckFailedScreens();
        });
    });

    connect(&TimeoutTimer, &QTimer::timeout, [this] {
        TimeoutTimer.stop();
        MakingScreen = false;
        WriteLog(LL_Warning, "failed to make screenshot: time out");
        emit OnFailed();
    });

    Server.setSocketOptions(QLocalServer::WorldAccessOption);
    if (!Server.listen("libqtscreen")) {
        WriteLog(LL_Error, "failed to listen named pipe libqtscreen - may be there is another libqtscreen server");
        return;
    }
    Initialized = true;
    WriteLog(LL_Info, "Successfuly initialized!");
    startTimer(500);
}

bool TScreenShotMaker::IsInitialized() const {
    return Initialized;
}

void TScreenShotMaker::MakeScreenshot(uint64_t pid) {
    if (!Initialized) {
        WriteLog(LL_Warning, "failed to make screenshot: not initialized");
        emit OnFailed();
        return;
    }

    if (Connections.size() == 0) {
        WriteLog(LL_Warning, "failed to make screenshot: missing any connected dll");
        emit OnFailed();
        return;
    }

    if (!FullScreenProcessID && !pid) {
        WriteLog(LL_Warning, "failed to make screenshot: no fullscreen app detected");
        emit OnFailed();
        return;
    }
    for (auto&& cli: Connections) {
        if (cli->GetInfo().PID == FullScreenProcessID ||
            cli->GetInfo().PID == pid)
        {
            MakingScreen = true;
            TimeoutTimer.start(1000);
            WriteLog(LL_Info, QString("making screenshot of process %1").arg(cli->GetInfo().PID));
            cli->MakeScreenshot();
            return;
        }
    }
    WriteLog(LL_Warning, "failed to make screenshot: missing connection to required process");
    emit OnFailed();
}

QImage TScreenShotMaker::GetLastScreenshot() {
    return LastScreenshot;
}

bool TScreenShotMaker::InjectToPID(uint64_t PID) {
    if (InjectedPIDs.find(PID) != InjectedPIDs.end()) {
        WriteLog(LL_Info, QString("already injected to %1").arg(PID));
        return true;
    }

    bool injected = false;
    if (IsOs64 && IsProcess64Bit(PID)) {
        injected = InjectDll(PID, Config.Helper64Path, GetFullPath(Config.DLL64Path));
    } else {
        injected = InjectDll(PID, Config.Helper32Path, GetFullPath(Config.DLL32Path));
    }
    if (injected) {
        WriteLog(LL_Info, QString("successfully injected to %1").arg(PID));
    } else {
        WriteLog(LL_Warning, QString("failed to inject to %1").arg(PID));
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
    std::set<uint32_t> prevPids;
    InjectedPIDs.swap(prevPids);
    for (auto&& c: Connections) {
        if (c->IsActive()) {
            newConnections.push_back(c);
            if (c->GetInfo().PID) {
                if (prevPids.find(c->GetInfo().PID) == prevPids.end()) {
                    WriteLog(LL_Info, QString("injected dll with pid %1 sent initial info").arg(c->GetInfo().PID));
                }
                InjectedPIDs.insert(uint64_t(c->GetInfo().PID));
            }
        } else {
            WriteLog(LL_Info, QString("injected dll with pid %1 disconnected").arg(c->GetInfo().PID));
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
        WriteLog(LL_Warning, "failed to make screenshot: injected dll was unable to make screenshot");
        emit OnFailed();
    }
}

void TScreenShotMaker::WriteLog(ELogLevel logLevel, const QString& message) {
    if (!Config.LogOutput || logLevel < Config.LogLevel) {
        return;
    }
    QString level;
    switch (logLevel) {
    case LL_Info:
        level = "INFO";
        break;
    case LL_Warning:
        level = "WARNING";
        break;
    case LL_Error:
        level = "ERROR";
        break;
    default:
        break;
    }
    QString msg = "[" + QDate::currentDate().toString("dd.MM.yy") + " " + QTime::currentTime().toString() + " " + level + "] " + message;
    (*Config.LogOutput) << msg << "\n";
    Config.LogOutput->flush();
}

} // NQtScreen
