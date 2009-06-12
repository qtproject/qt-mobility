
#include <private/qobject_p.h>

#include "qmediaplayer.h"

/*!
    \class QMediaPlayer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaPlayerPrivate : public QObjectPrivate
{
public:
    QMediaPlayerSession* session;
};


QMediaPlayer::QMediaPlayer(QMediaPlayerSession* session, QObject* parent):
    QObject(*new QMediaPlayerPrivate, parent)
{
    d_funct()->session = session;
}

QMediaPlayer::~QMediaPlayer()
{
}

QMediaPlayer::State QMediaPlayer::state() const
{
}

QMediaSource QMediaPlayer::source() const
{
}

qint64 QMediaPlayer::duration() const
{
}

qint64 QMediaPlayer::position() const
{
}

int QMediaPlayer::volume() const
{
}

bool QMediaPlayer::isMuted() const
{
}

bool QMediaPlayer::isBuffering() const
{
}

int QMediaPlayer::bufferStatus() const
{
}

bool QMediaPlayer::isVideoAvailable() const
{
}

QMediaPlayerSession* QMediaPlayer::session() const
{
}

//public Q_SLOTS:
void QMediaPlayer::setMediaSource(QMediaSource mediaSource)
{
}

void QMediaPlayer::play()
{
}

void QMediaPlayer::pause()
{
}

void QMediaPlayer::stop()
{
}

void QMediaPlayer::setPosition(qint64 position)
{
}

void QMediaPlayer::setVolume(int volume)
{
}

void QMediaPlayer::setMuted(bool muted)
{
}

