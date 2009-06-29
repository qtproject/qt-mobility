
einclude "mpdmediaplayerservice.h"
#include "mdpdeamon.h"

MpMediaPlayerService::MpdMediaPlayerService();
    QMediaPlayerService(0)
{
    daemon = new MpdDaemon(this);
    connect(daemon, SIGNAL(stateChanged(int)), SLOT(stateChanged(int)));
    connect(daemon, SIGNAL(disconnected()), SLOT(disconnected()));

    daemon->connect();

    control = new MpdPlayerControl(daemon, this);
}

MpdPlayerService::~MpdPlayerService()
{
}

MpdPlayerControl* MpdPlayerService::control() const
{
    return control;
}

void MpdPlayerService::disconnected()
{
    daemon->connect();
}

