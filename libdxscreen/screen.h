#include "client.h"

#include <QObject>
#include <QTimer>
#include <QImage>
#include <QLocalServer>
#include <vector>
#include <set>

class TScreenShotMaker: public QObject {
    Q_OBJECT
    friend class TClient;
public:
    TScreenShotMaker();
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
    QImage LastScreenshot;
    QLocalServer Server;
    std::vector<TClientRef> Connections;
    std::set<uint32_t> InjectedPIDs;
    TInjectorHelpInfo InjectorHelpInfo;
    QTimer TimeoutTimer;
    bool MakingScreen;
    uint32_t FullScreenProcessID;
};
