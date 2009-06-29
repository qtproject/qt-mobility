
#include "qmediaplayercontrol.h"
#include "qmediasource.h"

int QMediaPlayerControl::state() const
{
    return property("state").toInt();
}

void QMediaPlayerControl::setState(int state)
{
    changePropertyValue("state", state);
}

QMediaSource QMediaPlayerControl::mediaSource() const
{
    return property("mediaSource").value<QMediaSource>();
}

void QMediaPlayerControl::setMediaSource(QMediaSource mediaSource)
{
    changePropertyValue("mediaSource", qVariantFromValue(mediaSource));
}

qint64 QMediaPlayerControl::duration() const
{
    return property("duration").toLongLong();
}

void QMediaPlayerControl::setDuration(qint64 duration)
{
    changePropertyValue("duration", duration);
}

qint64 QMediaPlayerControl::position() const
{
    return property("position").toLongLong();
}

void QMediaPlayerControl::setPosition(qint64 position)
{
    changePropertyValue("position", position);
}

int QMediaPlayerControl::volume() const
{
    return property("volume").toInt();
}

void QMediaPlayerControl::setVolume(int volume)
{
    changePropertyValue("volume", volume);
}

bool QMediaPlayerControl::isMuted() const
{
    return property("muted").toBool();
}

void QMediaPlayerControl::setMuted(bool muted)
{
    changePropertyValue("muted", muted);
}

bool QMediaPlayerControl::isBuffering() const
{
    return property("buffering").toBool();
}

void QMediaPlayerControl::setBuffering(bool buffering)
{
    changePropertyValue("buffering", buffering);
}

int QMediaPlayerControl::bufferStatus() const
{
    return property("bufferStatus").toInt();
}

void QMediaPlayerControl::setBufferState(int bufferStatus)
{
    changePropertyValue("bufferStatus", bufferStatus);
}

bool QMediaPlayerControl::isVideoAvailable() const
{
    return property("videoAvailable").toBool();
}

void QMediaPlayerControl::setVideoAvailable(bool videoAvailable)
{
    changePropertyValue("videoAvailable", videoAvailable);
}


QMediaPlayerControl::QMediaPlayerControl(QObject *parent):
    QAbstractMediaControl(parent)
{
    addPropertyWatch("position");
}
