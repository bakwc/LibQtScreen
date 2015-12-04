#include "screener.h"

#include <libdxscreen/screen.h>

#include <QImage>


TScreener::TScreener(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    Screener.reset(new TScreenShotMaker());
    startTimer(1000);
    connect(Screener.get(), &TScreenShotMaker::OnScreenshotReady, [this] {
        QImage screenShot = Screener->GetLastScreenshot();
        qDebug() << "screenshot created" << screenShot.width();
    });
    connect(Screener.get(), &TScreenShotMaker::OnFailed, [this] {
        qDebug() << "screenshot failed";
    });
}

TScreener::~TScreener() {
}

void TScreener::timerEvent(QTimerEvent *) {
    static int n = 0;
    n += 1;
    if (n == 5) {
        qDebug() << "making screenshot";
        Screener->MakeScreenshot();
    }
}
