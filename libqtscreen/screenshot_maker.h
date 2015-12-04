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

    // Time to wait for screenshot, milliseconds.
    // OnFailed will be emmited after timeout expire.
    int Timeout = 300;
};

class TScreenShotMaker: public QObject {
    Q_OBJECT
    friend class TClient;
public:
    TScreenShotMaker(const TScreenShotMakerConfig& config = TScreenShotMakerConfig());
    void MakeScreenshot();
    QImage GetLastScreenshot();
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
    TInjectorHelpInfo InjectorHelpInfo;
    QTimer TimeoutTimer;
    bool MakingScreen;
    uint32_t FullScreenProcessID;
};

} // NQtScreen
