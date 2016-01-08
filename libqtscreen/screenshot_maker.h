#include "client.h"

#include <QObject>
#include <QTimer>
#include <QImage>
#include <QLocalServer>
#include <QString>
#include <QTextStream>
#include <vector>
#include <set>

namespace NQtScreen {

struct TScreenShotMakerConfig {
    // Path to x86 dll
    QString DLL32Path = "libqtscreen32.dll";

    // Path to x64 dll
    QString DLL64Path = "libqtscreen64.dll";

    // Path to x86 helper binary
    QString Helper32Path = "helper32.exe";

    // Path to x64 helper binary
    QString Helper64Path = "helper64.exe";

    // Time to wait for screenshot, milliseconds.
    // OnFailed will be emmited after timeout expire.
    int Timeout = 300;

    // Disable if you don't want to inject automatically.
    // In this case you should inject yourself using InjectToPID.
    bool AutoInjectToFullscreen = true;

    // Enable logs. Pass stdout or file stream pointer.
    QTextStream* LogOutput = nullptr;

    // Select log level.
    ELogLevel LogLevel = LL_Info;
};

class TScreenShotMaker: public QObject {
    Q_OBJECT
    friend class TClient;
public:
    TScreenShotMaker(const TScreenShotMakerConfig& config = TScreenShotMakerConfig());

    // Returns False in case of initialization issues.
    // Enable logs to see detailed information.
    bool IsInitialized() const;

    // Async request to make screenshot. Singals OnScreenshotReady
    // or OnFailed emitted wher ready (or fail).
    // pid (optional) - custom pid (for non-fullscreen apps), use InjectToPID before.
    void MakeScreenshot(uint64_t pid = 0);

    // Returns last successful screenshot. Use it to extract
    // screenshot when OnScreenshotReady emitted.
    QImage GetLastScreenshot();

    // Use it if you want to capture some non-fullscreen applications
    bool InjectToPID(uint64_t PID);
signals:
    void OnScreenshotReady();
    void OnFailed();
private:
    void timerEvent(QTimerEvent*);
    void InjectAll();
    void RemoveInactiveConnections();
    void CheckFailedScreens();
    bool GetOffsets(TInjectorHelpInfo& offsets, const QString& helperExe);
    void WriteLog(ELogLevel logLevel, const QString& message);
private:
    TScreenShotMakerConfig Config;
    QImage LastScreenshot;
    QLocalServer Server;
    std::vector<TClientRef> Connections;
    std::set<uint32_t> InjectedPIDs;
    TInjectorHelpInfo InjectorHelpInfo32;
    TInjectorHelpInfo InjectorHelpInfo64;
    QTimer TimeoutTimer;
    bool MakingScreen;
    uint32_t FullScreenProcessID;
    int32_t ProcessDetectedCounter;
    bool IsOs64;
    bool Initialized;
};

} // NQtScreen
