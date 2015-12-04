#pragma once

#include <QCoreApplication>
#include <memory>
#include <libqtscreen/screenshot_maker.h>

class TScreener : public QCoreApplication {
    Q_OBJECT
public:
    TScreener(int &argc, char **argv);
    virtual ~TScreener();
private:
    void timerEvent(QTimerEvent*);
    std::unique_ptr<NQtScreen::TScreenShotMaker> Screener;
};
