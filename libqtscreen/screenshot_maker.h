#include "client.h"

#include <QObject>
#include <QTimer>
#include <QImage>
#include <QLocalServer>
#include <QString>
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
};

class TScreenShotMaker: public QObject {
    Q_OBJECT
    friend class TClient;
public:
    TScreenShotMaker(const TScreenShotMakerConfig& config = TScreenShotMakerConfig());
    void MakeScreenshot();
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
    bool IsOs64;
};

} // NQtScreen
