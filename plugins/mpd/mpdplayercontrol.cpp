
#include "mpdplayercontrol.h"
#include "mpdplaylistsource.h"



MpdPlayerControl::MpdPlayerControl(MpdDaemon *mpd):
    QMediaPlayerControl(0),
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

void MpdPlayer::notify()
{
    if (duration() != daemon->duration())
        setDuration(daemon->playerDuration());
    if (position() != daemon->position())
        setPosition(daemon->position());
}

void MpdPlayer::playerChanged()
{
    if (state() != daemon->playerState())
        setState(daemon->playerState());
}

void MpdPlayer::mixerChanged()
{
    if (volume() != daemon->volume())
        setVolume(daemon->volume());
    if (muted() != (daemon->volume() == 0))
        setMuted(daemon->volume() == 0);
}
