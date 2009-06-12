
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


/*!
    Construct a QMediaPlayer to operate on the QMediaPlayerSession \a session, parented to \a parent.
*/

QMediaPlayer::QMediaPlayer(QMediaPlayerSession* session, QObject* parent):
    QObject(*new QMediaPlayerPrivate, parent)
{
    d_func()->session = session;
}

/*!
    Destroys the player object.
*/

QMediaPlayer::~QMediaPlayer()
{
}

/*!
   Returns the State of the Player object.
*/

QMediaPlayer::State QMediaPlayer::state() const
{
}

/*!
    Returns the current QMediaSource that is being operated on.
*/

QMediaSource QMediaPlayer::mediaSource() const
{
}

/*!
    Returns the total playback time in milliseconds of the current source.
*/

qint64 QMediaPlayer::duration() const
{
}

/*!
    Returns the current playback position in milliseconds from the beginning.
*/

qint64 QMediaPlayer::position() const
{
}

/*!
    Returns the current playback volume. Volume is a linear effect from 0-100.
*/

int QMediaPlayer::volume() const
{
}

/*!
    Returns true if the playback volume is muted; otherwise false.
*/

bool QMediaPlayer::isMuted() const
{
}

/*!
    Returns true if the source is being buffered; otherwise false.
*/

bool QMediaPlayer::isBuffering() const
{
}

/*!
    When buffering; returns the percent of the local buffer filled.
*/

int QMediaPlayer::bufferStatus() const
{
}

/*!
    Returns true if there is visual content available; otherwise false.
*/

bool QMediaPlayer::isVideoAvailable() const
{
}

/*!
    Returns the session object being controlled by this Player.
*/

QMediaPlayerSession* QMediaPlayer::session() const
{
}

//public Q_SLOTS:

/*!
    Set the Player object to operation on \a mediaSource as the current source.
*/

void QMediaPlayer::setMediaSource(QMediaSource mediaSource)
{
}

/*!
    Start or resume playing the current source.
*/

void QMediaPlayer::play()
{
}

/*!
    Pause playing the current source.
*/

void QMediaPlayer::pause()
{
}

/*!
    Stop playing, and reset the play position to the beginning.
*/

void QMediaPlayer::stop()
{
}

/*!
    Set the current play position to \a position, relative to the beginning.
    This method is not guaranteed to be synchronous.
*/

void QMediaPlayer::setPosition(qint64 position)
{
}

/*!
    Set the current volume [ 0 - 100] to  \a volume.
    This method is not guaranteed to be synchronous.
*/

void QMediaPlayer::setVolume(int volume)
{
}

/*!
    Set the mute state of the playback folume to \a muted. True to mute the
    playback volume, false to unmute.  This method is not guaranteed to be
    synchronous.
*/

void QMediaPlayer::setMuted(bool muted)
{
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
    \value EndOfStreamState The player object has stopped playing the content
                            as the content has reached a natural end.
*/

/*!
    \fn void QMediaPlayer::mediaSourceChanged(QMediaSource mediaSource)

    Signal that the player object has change its content to \a mediaSource.
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


