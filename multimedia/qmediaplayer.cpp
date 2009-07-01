/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmediaplayer.h"

#include "qabstractmediaobject_p.h"
#include "qmediaplayerservice.h"
#include "qmediaplayercontrol.h"
#include "qmediaserviceprovider.h"


/*!
    \class QMediaPlayer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaPlayerPrivate : public QAbstractMediaObjectPrivate
{
public:
    QMediaPlayerService* service;
    QMediaPlayerControl* control;
};


/*!
    Construct a QMediaPlayer to operate on the QMediaPlayerSession \a session, parented to \a parent.
*/

QMediaPlayer::QMediaPlayer(QMediaPlayerService *service, QObject *parent):
    QAbstractMediaObject(*new QMediaPlayerPrivate, parent)
{
    Q_ASSERT(service != 0);

    Q_D(QMediaPlayer);

    d->service = service;
    d->control = qobject_cast<QMediaPlayerControl *>(service->control("com.nokia.qt.MediaPlayerControl"));
    d->control->setNotifyObject(this);
}

/*!
    Destroys the player object.
*/

QMediaPlayer::~QMediaPlayer()
{
    Q_D(QMediaPlayer);

    delete d->control;
    delete d->service;
}

/*!
   Returns the State of the Player object.
*/

QMediaPlayer::State QMediaPlayer::state() const
{
    return QMediaPlayer::State(d_func()->control->state());
}

QMediaPlaylist* QMediaPlayer::mediaPlaylist() const
{
    return d_func()->control->mediaPlaylist();
}

/*!
    Returns the total playback time in milliseconds of the current source.
*/

qint64 QMediaPlayer::duration() const
{
    return d_func()->control->duration();
}

/*!
    Returns the current playback position in milliseconds from the beginning.
*/

qint64 QMediaPlayer::position() const
{
    return d_func()->control->position();
}

/*!
    Returns the current playback volume. Volume is a linear effect from 0-100.
*/

int QMediaPlayer::volume() const
{
    return d_func()->control->volume();
}

/*!
    Returns true if the playback volume is muted; otherwise false.
*/

bool QMediaPlayer::isMuted() const
{
    return d_func()->control->isMuted();
}

/*!
    Returns true if the source is being buffered; otherwise false.
*/

bool QMediaPlayer::isBuffering() const
{
    return d_func()->control->isBuffering();
}

/*!
    When buffering; returns the percent of the local buffer filled.
*/

int QMediaPlayer::bufferStatus() const
{
    return d_func()->control->bufferStatus();
}

/*!
    Returns true if there is visual content available; otherwise false.
*/

bool QMediaPlayer::isVideoAvailable() const
{
    return d_func()->control->isVideoAvailable();
}

/*!
    Returns the session object being controlled by this Player.
*/

QAbstractMediaService* QMediaPlayer::service() const
{
    return d_func()->service;
}

//public Q_SLOTS:

void QMediaPlayer::setMediaPlaylist(QMediaPlaylist *mediaPlaylist)
{
    d_func()->control->setMediaPlaylist(mediaPlaylist);
}

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
    Set the current volume [ 0 - 100] to  \a volume.
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

QMediaPlayerService* createMediaPlayerService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.MediaPlayer/1.0") : 0;

    if (object) {
        QMediaPlayerService *service = qobject_cast<QMediaPlayerService *>(object);

        if (service)
            return service;

        delete service;
    }
    return 0;
}

/*!
    \enum QMediaPlayer::State
    
    Defines the current state of the player object.

    \value LoadingState The player object is loading necessary data or libraries to play the content.
    \value PlayingState The player object is currently playing the content.
    \value PausedState The player object has paused playing the content, position information is, as necessary,
                        retained.
    \value StoppedState The player object has stopped playing the content, with
                        stop being requested by the stop() function, position information is retained but will
                        be reset on next call to play()
    \value SeekingState The player object is currently performing a seek operation.
    \value EndOfStreamState The player object has stopped playing the content
                            as the content has reached a natural end.
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
    \fn void QMediaPlayer::stateChanged(State newState)

    Signal the state of the Player object has changed to \a newState.
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


