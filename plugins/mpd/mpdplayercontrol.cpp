
#include <QtCore/qstringlist.h>

#include "mpdplayercontrol.h"
#include "mpdplaylistsource.h"
#include "mpddaemon.h"


MpdPlayerControl::MpdPlayerControl(MpdDaemon *mpd, QObject *parent):
    QMediaPlayerControl(parent),
    daemon(mpd)
{
    setMediaPlaylist(new QMediaPlaylist(new MpdPlaylistSource(daemon, this)));

    connect(daemon, SIGNAL(notify()), SLOT(notify()));
    connect(daemon, SIGNAL(playerChanged()), SLOT(playerChanged()));
    connect(daemon, SIGNAL(mixerChanged()), SLOT(mixerChanged()));
}

MpdPlayerControl::~MpdPlayerControl()
{
}

void MpdPlayerControl::play()
{
    daemon->send("play");
}

void MpdPlayerControl::pause()
{
    daemon->send("pause");
}

void MpdPlayerControl::stop()
{
    daemon->send("stop");
}

void MpdPlayerControl::notify()
{
    if (duration() != daemon->duration())
        setDuration(daemon->duration());
    if (position() != daemon->position())
        setPosition(daemon->position());
}

void MpdPlayerControl::playerChanged()
{
    if (state() != daemon->playerState())
        setState(daemon->playerState());
}

void MpdPlayerControl::mixerChanged()
{
    if (volume() != daemon->volume())
        setVolume(daemon->volume());
    if (isMuted() != (daemon->volume() == 0))
        setMuted(daemon->volume() == 0);
}
