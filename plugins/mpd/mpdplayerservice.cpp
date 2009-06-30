
#include "mpddaemon.h"
#include "mpdplayercontrol.h"
#include "mpdplayerservice.h"



MpdPlayerService::MpdPlayerService(QObject *parent):
    QMediaPlayerService(parent)
{
    daemon = new MpdDaemon(this);
    connect(daemon, SIGNAL(disconnected()), SLOT(disconnected()));

    playerControl = new MpdPlayerControl(daemon, this);
}

MpdPlayerService::~MpdPlayerService()
{
}

MpdPlayerControl* MpdPlayerService::control() const
{
    return playerControl;
}

void MpdPlayerService::stateChanged(int state)
{
}

void MpdPlayerService::disconnected()
{
}

