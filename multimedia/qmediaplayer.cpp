/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>

#include <QtCore/qcoreevent.h>

#include <multimedia/qmediaplayer.h>

#include <multimedia/qabstractmediaobject_p.h>
#include <multimedia/qabstractmediaservice.h>
#include <multimedia/qmediaplayercontrol.h>
#include <multimedia/qmediaserviceprovider.h>
#include <multimedia/qmediaplaylist.h>
#include <multimedia/qmediaplaylistcontrol.h>

/*!
    \class QMediaPlayer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaPlayerPrivate : public QAbstractMediaObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaPlayer)

public:
    QMediaPlayerPrivate():service(0), control(0) {}

    QAbstractMediaService* service;
    QMediaPlayerControl* control;
    QMediaPlayer::Error error;
    QString errorString;
    bool hasPlaylistControl;

    QMediaPlaylist *playlist;

    void _q_stateChanged(QMediaPlayer::State state);
    void _q_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void _q_error(int error, const QString &errorString);
    void _q_updateMedia(const QMediaSource&);
};

void QMediaPlayerPrivate::_q_stateChanged(QMediaPlayer::State ps)
{
    Q_Q(QMediaPlayer);

    if (ps == QMediaPlayer::PlayingState)
        q->addPropertyWatch("position");
    else
        q->removePropertyWatch("position");

    emit q->stateChanged(ps);
}

void QMediaPlayerPrivate::_q_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    Q_Q(QMediaPlayer);

    switch (status) {
    case QMediaPlayer::StalledMedia:
    case QMediaPlayer::BufferingMedia:
        q->addPropertyWatch("bufferStatus");
        emit q->mediaStatusChanged(status);
        break;
    case QMediaPlayer::EndOfMedia:
        q->removePropertyWatch("bufferStatus");
        if (playlist) {
            if (playlist->nextPosition(1) != -1) {
                playlist->advance();
            } else
                emit q->mediaStatusChanged(status);
        }
        break;
    default:
        q->removePropertyWatch("bufferStatus");
        emit q->mediaStatusChanged(status);
        break;
    }

}

void QMediaPlayerPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaPlayer);

    this->error = QMediaPlayer::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}

void QMediaPlayerPrivate::_q_updateMedia(const QMediaSource &media)
{
    Q_Q(QMediaPlayer);
    q->setMedia(media);
    q->play();
}


/*!
    Construct a QMediaPlayer to operate on the QMediaPlayerService \a service, parented to \a parent.

    If a playback service is not specified the system default will be used.
*/

QMediaPlayer::QMediaPlayer(QObject *parent, QMediaServiceProvider *provider):
    QAbstractMediaObject(*new QMediaPlayerPrivate, parent, provider->createService("mediaplayer"))
{
    Q_D(QMediaPlayer);

    Q_ASSERT(d->service != 0);

    if (d->service != 0) {
        d->control = qobject_cast<QMediaPlayerControl*>(d->service->control(QMediaPlayerControl_iid));
        if (d->control != 0) {
            connect(d->control, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(_q_stateChanged(QMediaPlayer::State)));
            connect(d->control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                    SLOT(_q_mediaStatusChanged(QMediaPlayer::MediaStatus)));
            connect(d->control, SIGNAL(error(int,QString)), SLOT(_q_error(int,QString)));

            connect(d->control, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged(qint64)));
            connect(d->control, SIGNAL(positionChanged(qint64)), SIGNAL(positionChanged(qint64)));
            connect(d->control, SIGNAL(videoAvailabilityChanged(bool)), SIGNAL(videoAvailabilityChanged(bool)));
            connect(d->control, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged(int)));
            connect(d->control, SIGNAL(mutingChanged(bool)), SIGNAL(mutingChanged(bool)));
            connect(d->control, SIGNAL(seekableChanged(bool)), SIGNAL(seekableChanged(bool)));

            if (d->control->state() == PlayingState)
                addPropertyWatch("position");

            if (d->control->mediaStatus() == StalledMedia || d->control->mediaStatus() == BufferingMedia)
                addPropertyWatch("bufferStatus");

            d->hasPlaylistControl = (d->service->control(QMediaPlaylistControl_iid) != 0);
        }
    }
}


/*!
    Destroys the player object.
*/

QMediaPlayer::~QMediaPlayer()
{
}

/*!
    \reimp
*/

bool QMediaPlayer::isValid() const
{
    return QAbstractMediaObject::isValid() && d_func()->control != 0;
}

QMediaSource QMediaPlayer::media() const
{
    return d_func()->control->media();
}

/*!
    Returns the stream source of media data.

    This is only valid if a stream was passed to setMedia().

    \sa setMedia()
*/

const QIODevice *QMediaPlayer::mediaStream() const
{
    return d_func()->control->mediaStream();
}

QMediaPlayer::State QMediaPlayer::state() const
{
    return QMediaPlayer::State(d_func()->control->state());
}

QMediaPlayer::MediaStatus QMediaPlayer::mediaStatus() const
{
    return QMediaPlayer::MediaStatus(d_func()->control->mediaStatus());
}

qint64 QMediaPlayer::duration() const
{
    return d_func()->control->duration();
}

qint64 QMediaPlayer::position() const
{
    return d_func()->control->position();
}

int QMediaPlayer::volume() const
{
    return d_func()->control->volume();
}

bool QMediaPlayer::isMuted() const
{
    return d_func()->control->isMuted();
}

int QMediaPlayer::bufferStatus() const
{
    return d_func()->control->bufferStatus();
}

bool QMediaPlayer::isVideoAvailable() const
{
    return d_func()->control->isVideoAvailable();
}

bool QMediaPlayer::isSeekable() const
{
    return d_func()->control->isSeekable();
}

float QMediaPlayer::playbackRate() const
{
    return d_func()->control->playbackRate();
}

/*!
    Returns the current error state.
*/

QMediaPlayer::Error QMediaPlayer::error() const
{
    return d_func()->error;
}

QString QMediaPlayer::errorString() const
{
    return d_func()->errorString;
}

//public Q_SLOTS:
/*!
    Start or resume playing the current source.
*/

void QMediaPlayer::play()
{
    Q_D(QMediaPlayer);

    if (d->playlist &&
        d->playlist->currentPosition() == -1 &&
        !d->playlist->isEmpty())
    {
        d->playlist->setCurrentPosition(0);
        return;
    }

    // Reset error conditions
    d->error = NoError;
    d->errorString = QString();

    d->control->play();
}

/*!
    Pause playing the current source.
*/

void QMediaPlayer::pause()
{
    d_func()->control->pause();
}

/*!
    Stop playing, and reset the play position to the beginning.
*/

void QMediaPlayer::stop()
{
    d_func()->control->stop();
}

void QMediaPlayer::setPosition(qint64 position)
{
    if (!isSeekable())
        return;

    d_func()->control->setPosition(qBound(qint64(0), duration(), position));
}

void QMediaPlayer::setVolume(int v)
{
    int clamped = qBound(0, v, 100);
    if (clamped == volume())
        return;

    d_func()->control->setVolume(clamped);
}

void QMediaPlayer::setMuted(bool muted)
{
    if (muted == isMuted())
        return;

    d_func()->control->setMuted(muted);
}

void QMediaPlayer::setPlaybackRate(float rate)
{
    d_func()->control->setPlaybackRate(rate);
}

/*!
    Sets the current \a media source.

    If a \a stream is supplied; media data will be read from it instead of resolving the media
    source.  In this case the media source may still be used to resolve additional information
    about the media such as mime type.
*/
void QMediaPlayer::setMedia(const QMediaSource &media, QIODevice *stream)
{
    d_func()->control->setMedia(media, stream);
}

void QMediaPlayer::bind(QObject *obj)
{
    Q_D(QMediaPlayer);

    if (!d->hasPlaylistControl) {
        QMediaPlaylist *playlist = qobject_cast<QMediaPlaylist*>(obj);

        if (playlist) {
            qDebug() << "playlist attached";
            d->playlist = playlist;
            connect(d->playlist, SIGNAL(currentMediaChanged(QMediaSource)),
                    this, SLOT(_q_updateMedia(QMediaSource)));
        }
    }
}


// Enums
/*!
    \enum QMediaPlayer::State

    Defines the current state of a media player.

    \value PlayingState The media player is currently playing content.
    \value PausedState The media player has paused playback, playback of the current track will
    resume from the position the player was paused at.
    \value StoppedState The media player is not playing content, playback will begin from the start
    of the current track.
*/

/*!
    \enum QMediaPlayer::MediaStatus

    Defines the status of a media player's current media.

    \value UnknownMediaStatus The status of the media cannot be determined.
    \value NoMedia The is no current media.  The player is in the StoppedState.
    \value LoadingMedia The current media is being loaded. The player may be in any state.
    \value LoadedMedia The current media has been loaded. The player is in the StoppedState.
    \value StalledMedia Playback of the current media has stalled due to insufficient buffering or
    some other temporary interruption.  The player is in the PlayingState or PausedState.
    \value BufferingMedia The player is buffering data but has enough data buffered for playback to
    continue for the immediate future.  The player is in the PlayingState or PausedState.
    \value BufferedMedia The player has fully buffered the current media.  The player is in the
    PlayingState or PausedState.
    \value EndOfMedia Playback has reached the end of the current media.  The player is in the
    StoppedState.
    \value InvalidMedia The current media cannot be played.  The player is in the StoppedState.
*/

/*!
    \enum QMediaPlayer::Error

    Defines a media player error condition.

    \value NoError No error has occurred.
    \value ResourceError A media resource couldn't be resolved.
    \value FormatError The format of a media resource isn't (fully) supported.  Playback may still
    be possible, but without an audio or video component.
    \value NetworkError A network error occurred.
    \value AccessDeniedError There are not the appropriate permissions to play a media resource
*/

// Signals
/*!
    \fn QMediaPlayer::error(QMediaPlayer::Error error)

    Signals that an \a error condition has occurred.

    \sa errorString()
*/

/*!
    \fn void QMediaPlayer::stateChanged(State state)

    Signal the \a state of the Player object has changed.
*/

/*!
    \fn QMediaPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)

    Signals that the \a status of the current media has changed.

    \sa mediaStatus()
*/

/*!
    \fn void QMediaPlayer::mediaChanged(const QMediaSource &media);

    Signals that the current playing content will be obtained from \a media.

    \sa media()
*/

/*!
    \fn void QMediaPlayer::playbackRateChanged(float rate);

    Signals the playbackRate has changed to \a rate.
*/

/*!
    \fn void QMediaPlayer::seekableChanged(bool seekable);

    Signals the \a seekable status of the player object has changed.
*/

// Properties
/*!
    \property QMediaPlayer::state
    \brief the media player's playback state.

    By default this property is QMediaPlayer::Stopped

    \sa mediaStatus(), play(), pause(), stop()
*/

/*!
    \property QMediaPlayer::error
    \brief a string describing the last error condition.

    \sa error()
*/

/*!
    \property QMediaPlayer::media
    \brief The media currently being by the player object.

    The player object will use the QMediaSource for selection of the content to
    be played.

    By default this property has a null QMediaSource.

    \sa QMediaSource
*/

/*!
    \property QMediaPlayer::mediaStatus
    \brief the status of the current media stream.

    The stream status describes how the playback of the current stream is progressing.

    By default this property is QMediaPlayer::NoMedia

    \sa state
*/

/*!
    \property QMediaPlayer::duration
    \brief The total playback time in milliseconds of the current source.
*/


/*!
    \property QMediaPlayer::position
    \brief The current playback position in milliseconds from the beginning.
*/

/*!
    \property QMediaPlayer::volume
    \brief The current playback volume. Volume is a linear effect from 0-100.
*/

/*!
    \property QMediaPlayer::muted
    \brief true if the playback volume is muted; otherwise false.
*/

/*!
    \property QMediaPlayer::bufferStatus
    \brief When buffering; returns the percent of the local buffer filled.
*/

/*!
    \property QMediaPlayer::videoAvailable
    \brief true if there is visual content available; otherwise false.
*/

/*!
    \property QMediaPlayer::seekable
    \brief true if the currently playing content is seekable; otherwise false.
*/

/*!
    \property QMediaPlayer::playbackRate
    \brief the rate of playback
*/

/*!
    \fn void QMediaPlayer::durationChanged(qint64 duration)

    Signal the duration of the content has changed to \a duration, expressed in milliseconds.
*/

/*!
    \fn void QMediaPlayer::positionChanged(qint64 position)

    Signal the position of the content has changed to \a position, expressed in
    milliseconds approximately every 1000milliseconds.
*/

/*!
    \fn void QMediaPlayer::volumeChanged(int volume)

    Signal the playback volume has changed to \a volume.
*/

/*!
    \fn void QMediaPlayer::mutingChanged(bool muted)

    Signal the mute state has changed to \a muted.
*/

/*!
    \fn void QMediaPlayer::videoAvailabilityChanged(bool videoAvailable)

    Signal the availability of visual cntent has changed to \a videoAvailable.
*/

/*!
    \fn void QMediaPlayer::bufferStatusChanged(int percentFilled)

    Signal the amount of the local buffer filled as a percentage by \a percentFilled.
*/

#include "moc_qmediaplayer.cpp"
