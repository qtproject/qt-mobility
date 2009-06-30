
#ifndef MPDPLAYERSERVICE_H
#define MPDPLAYERSERVICE_H

#include <qmediaplayerservice.h>
#include "mpdplayercontrol.h"

class MpdDaemon;

class MpdPlayerService : public QMediaPlayerService
{
    Q_OBJECT

public:
    MpdPlayerService(QObject *parent = 0);
    ~MpdPlayerService();

    virtual MpdPlayerControl* control() const;

private slots:
    void stateChanged(int state);
    void disconnected();

private:
    MpdDaemon *daemon;
    MpdPlayerControl *playerControl;
};

#endif  // MPDPLAYERSERVICE_H
