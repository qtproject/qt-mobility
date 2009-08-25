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

#include "qmediaplayer.h"

#include "qabstractmediaobject_p.h"
#include "qmediaplayerservice.h"
#include "qmediaplayercontrol.h"
#include "qmediaserviceprovider.h"
#include "qmediaplaylist.h"

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
    QMediaPlayerService* service;
    QMediaPlayerControl* control;
    QMediaPlayer::Error error;
    QString errorString;

    void _q_stateChanged(QMediaPlayer::State state);
    void _q_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void _q_error(int error, const QString &errorString);
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
        break;
    default:
        q->removePropertyWatch("bufferStatus");
        break;
    }
    emit q->mediaStatusChanged(status);
}

void QMediaPlayerPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaPlayer);

    this->error = QMediaPlayer::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
    emit q->errorStringChanged(this->errorString);
}

/*!
    Construct a QMediaPlayer to operate on the QMediaPlayerService \a service, parented to \a parent.
*/

QMediaPlayer::QMediaPlayer(QMediaPlayerService *service, QObject *parent):
    QAbstractMediaObject(*new QMediaPlayerPrivate, parent)
{
    Q_ASSERT(service != 0);

    Q_D(QMediaPlayer);

    d->service = service;
    d->control = qobject_cast<QMediaPlayerControl *>(service->control(QMediaPlayerControl_iid));

    connect(d->control, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(_q_stateChanged(QMediaPlayer::State)));
    connect(d->control, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            SLOT(_q_mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(d->control, SIGNAL(error(int,QString)), this, SLOT(_q_error(int,QString)));

    connect(d->control, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged(qint64)));
    connect(d->control, SIGNAL(videoAvailabilityChanged(bool)), SIGNAL(videoAvailabilityChanged(bool)));
    connect(d->control, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged(int)));
    connect(d->control, SIGNAL(mutingChanged(bool)), SIGNAL(mutingChanged(bool)));
    connect(d->control, SIGNAL(seekableChanged(bool)), SIGNAL(seekableChanged(bool)));


    if (d->control->state() == PlayingState)
        addPropertyWatch("position");

    if (d->control->mediaStatus() == StalledMedia || d->control->mediaStatus() == BufferingMedia)
        addPropertyWatch("bufferStatus");
}

/*!
    Destroys the player object.
*/

QMediaPlayer::~QMediaPlayer()
{
    Q_D(QMediaPlayer);

    delete d->service;
}

/*!
    \reimp
*/
bool QMediaPlayer::isValid() const
{
    return d_func()->service;
}


QMediaResourceList QMediaPlayer::currentMediaResources() const
{
    return d_func()->control->currentResources();
}

/*!
    \property QMediaPlayer::state
    \brief the media player's playback state.

    By default this property is QMediaPlayer::Stopped

    \sa mediaStatus(), play(), pause(), stop()
*/

/*!
    \fn void QMediaPlayer::stateChanged(State state)

    Signal the \a state of the Player object has changed.
*/

QMediaPlayer::State QMediaPlayer::state() const
{
    return QMediaPlayer::State(d_func()->control->state());
}

/*!
    \property QMediaPlayer::mediaStatus
    \brief the status of the current media stream.

    The stream status describes how the playback of the current stream is progressing.

    By default this property is QMediaPlayer::NoMedia

    \sa state
*/

/*!
    \fn QMediaPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)

    Signals that the \a status of the current media has changed.

    \sa mediaStatus()
*/

QMediaPlayer::MediaStatus QMediaPlayer::mediaStatus() const
{
    return QMediaPlayer::MediaStatus(d_func()->control->mediaStatus());
}


/*!
  Returns duration of currently played source in miliseconds.
*/
qint64 QMediaPlayer::duration() const
{
    return d_func()->control->duration();
}

/*!
  Returns position in miliseconds the current source plays at.
*/
qint64 QMediaPlayer::position() const
{
    return d_func()->control->position();
}

/*!
  Returns playback volume, in range 0..100.
*/
int QMediaPlayer::volume() const
{
    return d_func()->control->volume();
}

/*!
  Returns true is audio is muted; otherwise returns false.
*/
bool QMediaPlayer::isMuted() const
{
    return d_func()->control->isMuted();
}

/*!
  Returns how much the buffer is filled in percent.
*/
int QMediaPlayer::bufferStatus() const
{
    return d_func()->control->bufferStatus();
}

/*!
  Returns true if the current media contains video data; otherwise, returns false.
*/
bool QMediaPlayer::isVideoAvailable() const
{
    return d_func()->control->isVideoAvailable();
}

/*!
  Returns true if playback position can be changed by setPosition(); otherwise, returns false.
*/
bool QMediaPlayer::isSeekable() const
{
    return d_func()->control->isSeekable();
}

float QMediaPlayer::playbackRate() const
{
    return d_func()->control->playbackRate();
}

/*!
    Returns the last error condition.

    \sa errorString(), unsetError()
*/
QMediaPlayer::Error QMediaPlayer::error() const
{
    return d_func()->error;
}

/*!
    \fn QMediaPlayer::error(QMediaPlayer::Error error)

    Signals that an \a error condition has occured.

    \sa errorString()
*/

/*!
    \property QMediaPlayer::error
    \brief a string describing the last error condition.

    \sa error()
*/

/*!
    \fn QMediaPlayer::errorStringChanged(const QString &string)

    Signals that the \a string describing the last error condition has changed.

    \sa errorString()
*/

QString QMediaPlayer::errorString() const
{
    return d_func()->errorString;
}

/*!
    Resets the value of error() to NoError, and clears the value or errorString().

    \sa error() errorString()
*/
void QMediaPlayer::unsetError()
{
    Q_D(QMediaPlayer);

    d->error = NoError;
    d->errorString = QString();
}

/*!
    Returns the session object being controlled by this Player.
*/

QAbstractMediaService* QMediaPlayer::service() const
{
    return d_func()->service;
}

//public Q_SLOTS:
/*!
    Start or resume playing the current source.
*/
void QMediaPlayer::play()
{
    d_func()->control->play();
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

/*!
    Set the current play position to \a position, relative to the beginning.
    This method is not guaranteed to be synchronous.
*/
void QMediaPlayer::setPosition(qint64 position)
{
    d_func()->control->setPosition(position);
}

/*!
    Set the current volume [ 0 - 100 ] to  \a volume.
    This method is not guaranteed to be synchronous.
*/
void QMediaPlayer::setVolume(int volume)
{
    d_func()->control->setVolume(volume);
}

/*!
    Set the mute state of the playback folume to \a muted. True to mute the
    playback volume, false to unmute.  This method is not guaranteed to be
    synchronous.
*/
void QMediaPlayer::setMuted(bool muted)
{
    d_func()->control->setMuted(muted);
}

void QMediaPlayer::setPlaybackRate(float rate)
{
    d_func()->control->setPlaybackRate(rate);
}

QMediaPlayerService* createMediaPlayerService(QMediaServiceProvider *provider)
{
    if (!provider) {
        QByteArray providerKey = qgetenv("QT_MEDIAPLAYER_PROVIDER");

        provider = defaultServiceProvider(!providerKey.isNull()
                ? providerKey.constData()
                : "mediaplayer");
    }

    QObject *object = provider ? provider->createObject(QMediaPlayerService_iid) : 0;

    if (object != 0) {
        QMediaPlayerService *service = qobject_cast<QMediaPlayerService*>(object);

        if (service != 0)
            return service;

        delete object;
    }

    return 0;
}

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

/*!
    \property QMediaPlayer::mediaPlaylist
    \brief The QMediaPlaylist being used for playback.
*/

/*!
    \property QMediaPlayer::mediaSource
    \brief The resource of the active media.

    \sa playlistPosition()
*/

/*!
    \property QMediaPlayer::playlistPosition
    \brief The position of the current playing item in the playlist.
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
    \property QMediaPlayer::buffering
    \brief true if the source is being buffered; otherwise false.
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
    \fn void QMediaPlayer::playlistPositionChanged(int playlistPosition)

    Signal the current playing item as being changes to the item at index \a
    playlistPosition, in the players QMediaPlaylist.

    \sa playlistPosition(), setPlaylistPosition()
*/

/*!
    \fn void QMediaPlayer::currentMediaResourcesChanged(const QMediaResourceList &resources)

    Signals that the current media \a resources have changed.

    \sa playlistPositionChanged()
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
    \fn void QMediaPlayer::bufferingChanged(bool buffering)

    Signal the state of buffering has changed to \a buffering. When true some
    amount of the content is being buffered locally; when false local buffering
    has stopped.
*/

/*!
    \fn void QMediaPlayer::bufferStatusChanged(int percentFilled)

    Signal the amount of the local buffer filled as a percentage by \a percentFilled.
*/

#include "moc_qmediaplayer.cpp"
