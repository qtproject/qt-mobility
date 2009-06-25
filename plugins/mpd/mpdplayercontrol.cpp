
#include "mpdplayercontrol.h"




MpdPlayerControl::MpdPlayerControl(MpdDaemon *mpd):
    QMediaPlayerControl(0),
    daemon(mpd)
{
    connect(daemoin, SIGNAL(notify()), SLOT(checkStatus()));

    idle();
}

MpdPlayerControl::~MpdPlayerControl()
{
}

void MpdPlayerControl::setMediaSource(QMediaSource source)
{
    // Map mediasource to play id
}

void MpdPlayerControl::play()
{
    daemon->play();
}

void MpdPlayerControl::pause()
{
    daemon->pause();
}

void MpdPlayerControl::stop()
{
    daemon->stop();
}

void MpdPlayer::checkStatus()
{
    if (state() != daemon->playerState())
        setState(daemon->playerState());
    if (duration() != daemon->duration())
        setDuration(daemon->playerDuration());
    if (volume() != daemon->volume())
        setVolume(daemon->volume());
    if (muted() != (daemon->volume() == 0))
        setMuted(daemon->volume() == 0);
    if (position() != daemon->position())
        setPosition(daemon->position());
}


