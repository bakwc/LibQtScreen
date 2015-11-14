#include "client.h"

#include <QObject>
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
private:
    void timerEvent(QTimerEvent*);
    void InjectAll();
private:
    QImage LastScreenshot;
    QLocalServer Server;
    std::vector<TClientRef> Connections;
    std::set<uint64_t> InjectedPIDs;
    TInjectorHelpInfo InjectorHelpInfo;
    int N = 0;
};
