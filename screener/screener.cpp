#include "screener.h"

#include <QImage>

TScreener::TScreener(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    Screener.reset(new NQtScreen::TScreenShotMaker());
    startTimer(1000);
    connect(Screener.get(), &NQtScreen::TScreenShotMaker::OnScreenshotReady, [this] {
        QImage screenShot = Screener->GetLastScreenshot();
        qDebug() << "screenshot created" << screenShot.width();
    });
    connect(Screener.get(), &NQtScreen::TScreenShotMaker::OnFailed, [this] {
        qDebug() << "screenshot failed";
    });
}

TScreener::~TScreener() {
}

void TScreener::timerEvent(QTimerEvent *) {
    static int n = 10;
    n -= 1;
    if (n > 0) {
        qDebug() << "screenshot in" << n;
    }
    if (n == 0) {
        qDebug() << "making screenshot";
        Screener->MakeScreenshot();
    }
}
