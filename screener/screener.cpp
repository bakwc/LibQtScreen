#include "screener.h"

#include <QImage>

TScreener::TScreener(int &argc, char **argv)
    : QCoreApplication(argc, argv)
{
    startTimer(1000);
    connect(&Screener, &TScreenShotMaker::OnScreenshotReady, [this] {
        QImage screenShot = Screener.GetLastScreenshot();
        qDebug() << "screenshot created" << screenShot.width();
    });
}

void TScreener::timerEvent(QTimerEvent *) {
    static int n = 0;
    n += 1;
    if (n == 10) {
        qDebug() << "making screenshot";
        Screener.MakeScreenshot();
    }
}
