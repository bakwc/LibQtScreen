#pragma once

#include <QCoreApplication>

#include <libdxscreen/screen.h>

class TScreener : public QCoreApplication {
    Q_OBJECT
public:
    TScreener(int &argc, char **argv);
private:
    void timerEvent(QTimerEvent*);
    TScreenShotMaker Screener;
};
