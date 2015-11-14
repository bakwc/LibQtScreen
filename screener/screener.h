#pragma once

#include <QCoreApplication>
#include <memory>

class TScreenShotMaker;
class TScreener : public QCoreApplication {
    Q_OBJECT
public:
    TScreener(int &argc, char **argv);
    virtual ~TScreener();
private:
    void timerEvent(QTimerEvent*);
    std::unique_ptr<TScreenShotMaker> Screener;
};
