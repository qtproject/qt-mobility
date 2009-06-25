
einclude "mpdmediaplayerservice.h"
#include "mdpdeamon.h"

MpMediaPlayerService::MpdMediaPlayerService();
    QMediaPlayerService(0)
{
    daemon = new MpdDaemon(this);
    connect(daemon, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
    connect(daemon, SIGNAL(disconnected()), SLOT(disconnected()));

    daemon->connect();
}

MpdPlayerService::~MpdPlayerService()
{
}

MpdPlayerService* MpdPlayerService::control() const
{
    return new MpdPlayerControl(daemon);
}

void MpdPlayerService::disconnected()
{
    daemon->connect();
}

