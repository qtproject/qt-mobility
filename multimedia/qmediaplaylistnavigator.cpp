#include "qmediaplaylistnavigator.h"
#include "qmediaplaylist.h"

#include <QtCore/private/qobject_p.h>
#include <QtCore/qdebug.h>
#include "qmediasource.h"

class QMediaPlaylistNavigatorPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QMediaPlaylistNavigator)
public:
    QMediaPlaylistNavigatorPrivate()
        :playlist(0),
        currentPos(-1),        
        playbackMode(QMediaPlaylistNavigator::NoPlayback),
        randomPositionsOffset(-1),
        currentPosAfterListModifications(0)
    {
    }

    QMediaPlaylist *playlist;
    int currentPos;
    QMediaPlaylistNavigator::PlaybackMode playbackMode;
    QMediaSource currentItem;

    mutable QList<int> randomModePositions;
    mutable int randomPositionsOffset;

    int currentPosAfterListModifications;

    int nextItemPos(int steps = 1) const;
    int previousItemPos(int steps = 1) const;

    void _q_itemsInserted(int start, int end);
    void _q_itemsRemoved(int start, int end);
    void _q_itemsChanged(int start, int end);
    void _q_updateCurrentItemPos();
};


int QMediaPlaylistNavigatorPrivate::nextItemPos(int steps) const
{
    if (playlist->size() == 0)
        return -1;

    if (steps == 0)
        return currentPos;

    switch (playbackMode) {
        case QMediaPlaylistNavigator::NoPlayback:
        case QMediaPlaylistNavigator::CurrentItemOnce:
            return -1;
        case QMediaPlaylistNavigator::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylistNavigator::Linear:
            {
                int nextPos = currentPos+steps;
                return nextPos < playlist->size() ? nextPos : -1;
            }
        case QMediaPlaylistNavigator::Loop:
            return (currentPos+steps) % playlist->size();
        case QMediaPlaylistNavigator::Random:
            {
                //TODO: limit the history size

                while (randomModePositions.size() < randomPositionsOffset+steps+1)
                    randomModePositions.append(-1);
                int res = randomModePositions[randomPositionsOffset+steps];
                if (res<0 || res >= playlist->size()) {
                    res = qrand() % playlist->size();
                    randomModePositions[randomPositionsOffset+steps] = res;
                }

                return res;
            }
    }

    return -1;
}

int QMediaPlaylistNavigatorPrivate::previousItemPos(int steps) const
{
    if (playlist->size() == 0)
        return -1;

    if (steps == 0)
        return currentPos;

    switch (playbackMode) {
        case QMediaPlaylistNavigator::NoPlayback:
        case QMediaPlaylistNavigator::CurrentItemOnce:
            return -1;
        case QMediaPlaylistNavigator::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylistNavigator::Linear:
            {
                int prevPos = currentPos - steps;
                return prevPos>=0 ? prevPos : -1;
            }
        case QMediaPlaylistNavigator::Loop:
            {
                int prevPos = currentPos - steps;
                while (prevPos<0)
                    prevPos += playlist->size();
                return prevPos;
            }
        case QMediaPlaylistNavigator::Random:
            {
                //TODO: limit the history size

                while (randomPositionsOffset-steps < 0) {
                    randomModePositions.prepend(-1);
                    randomPositionsOffset++;
                }

                int res = randomModePositions[randomPositionsOffset-steps];
                if (res<0 || res >= playlist->size()) {
                    res = qrand() % playlist->size();
                    randomModePositions[randomPositionsOffset-steps] = res;
                }

                return res;
            }
    }

    return -1;
}


/*!
enum QMediaPlaylistNavigator::PlaybackMode

\item
    NoPlayback No item is playing right now.

\item
    CurrentItemOnce The current item is played only once.

\item
    CurrentItemInLoop The current item is played in the loop.

\item Linear
    Playback starts from the first to the last items and stops.
    QMediaPlaylistNavigator::nextItem() returns null item when the last
    one is currently playing.

\item Loop
    Playback continues from the first item after the last one finished playing.

\item Random
    Play items in random order.
*/


/*!
  Create a new \a playlist navigator object.
  */
QMediaPlaylistNavigator::QMediaPlaylistNavigator(QMediaPlaylist *playlist, QObject *parent)
    :QObject(*new QMediaPlaylistNavigatorPrivate, parent)
{
    Q_D(QMediaPlaylistNavigator);
    d->playlist = playlist;

    connect(playlist, SIGNAL(itemsAboutToBeInserted(int,int)), SLOT(_q_itemsInserted(int,int)));
    connect(playlist, SIGNAL(itemsInserted()), SLOT(_q_updateCurrentItemPos()));
    connect(playlist, SIGNAL(itemsAboutToBeRemoved(int,int)), SLOT(_q_itemsRemoved(int,int)));
    connect(playlist, SIGNAL(itemsRemoved()), SLOT(_q_updateCurrentItemPos()));

    connect(playlist, SIGNAL(itemsChanged(int,int)), SLOT(_q_itemsChanged(int,int)));
}

/*!
  Destroys the playlist.
  */
QMediaPlaylistNavigator::~QMediaPlaylistNavigator()
{
}


/*!
  */
QMediaPlaylistNavigator::PlaybackMode QMediaPlaylistNavigator::playbackMode() const
{
    return d_func()->playbackMode;
}

/*!
  */
void QMediaPlaylistNavigator::setPlaybackMode(QMediaPlaylistNavigator::PlaybackMode mode)
{
    Q_D(QMediaPlaylistNavigator);
    if (d->playbackMode == mode)
        return;

    if (mode == Random) {
        d->randomPositionsOffset = 0;
        d->randomModePositions.append(d->currentPos);
    } else if (d->playbackMode == Random) {
        d->randomPositionsOffset = -1;
        d->randomModePositions.clear();
    }

    d->playbackMode = mode;

    emit playbackModeChanged(mode);

    if (mode == NoPlayback)
        emit activated(QMediaSource());
}

/*!
  Returns the current playlist item.
  It is the same as itemAt(currentPosition());

  \sa currentPosition()
  */
QMediaSource QMediaPlaylistNavigator::currentItem() const
{
    return itemAt(d_func()->currentPos);
}

/*!
  */
QMediaSource QMediaPlaylistNavigator::nextItem(int steps) const
{
    return itemAt(nextPosition(steps));
}

/*!
  */
QMediaSource QMediaPlaylistNavigator::previousItem(int steps) const
{
    return itemAt(previousPosition(steps));
}

/*!
  Returns the media source at playlist position \a pos or
  invalid media source object if \a pos is out the playlist positions range.
  */
QMediaSource QMediaPlaylistNavigator::itemAt(int pos) const
{
    if ( pos<0 || pos>=d_func()->playlist->size() )
        return QMediaSource();
    else
        return d_func()->playlist->itemAt(pos);
}

/*!
  Returns the current items position in the playlist
  or -1 if no items should be currently played.

  \sa nextPosition(int), previousPosition(int), seek(int)
  */
int QMediaPlaylistNavigator::currentPosition() const
{
    return d_func()->currentPos;
}

/*!
  Returns the next position in the playlist to be played or -1 if playback should be stopped.
  It's usually but not necessary currentPosition()+1 depending on playbackMode().

  For example, for CurrentItemInLoop playback mode it's always the same as currentPosition().

  If \a steps parameter is passed, the item position expected after \a step advance() actions returned.

  \sa currentPosition(), previousPosition(int), PlaybackMode
  */
int QMediaPlaylistNavigator::nextPosition(int steps) const
{
    return d_func()->nextItemPos(steps);
}

/*!
  Returns the previously position in playlist.
  It's usually but not necessary currentPosition()-1 depending on playbackMode().

  If \a steps parameter is passed, the item position expected after \a step back() actions returned.

  \sa nextPosition
  */
int QMediaPlaylistNavigator::previousPosition(int steps) const
{
    return d_func()->previousItemPos(steps);
}

/*!
  Advance to the next item in the playlist.

  \sa back(), seek(int), QMediaPlaylistNavigator::PlaybackMode
  */
void QMediaPlaylistNavigator::advance()
{
    Q_D(QMediaPlaylistNavigator);

    int nextPos = d->nextItemPos();
    if (nextPos >= 0) {
        if ( playbackMode() == Random )
            d->randomPositionsOffset++;

        jump(nextPos);

        if (playbackMode() == CurrentItemInLoop ||
            (playbackMode() == Loop && d->playlist->size() == 1)) {
                emit activated(d->currentItem);
        }
    }
}

/*!
  Advance to the previously item in the playlist,
  depending on playback mode.

  \sa advance(), seek(int), QMediaPlaylistNavigator::PlaybackMode
  */
void QMediaPlaylistNavigator::back()
{
    Q_D(QMediaPlaylistNavigator);

    int prevPos = d->previousItemPos();
    if (prevPos >= 0) {
        if ( playbackMode() == Random )
            d->randomPositionsOffset--;

        jump(prevPos);

        if (playbackMode() == CurrentItemInLoop ||
            (playbackMode() == Loop && d->playlist->size() == 1)) {
                emit activated(d->currentItem);
        }
    }
}

/*!
  Jump to the item at position \a pos.
  The item is activated in all the modes except of NoPlayback.
  */
void QMediaPlaylistNavigator::jump(int pos)
{
    Q_D(QMediaPlaylistNavigator);

    if (pos<0 || pos>=d->playlist->size()) {
        qWarning() << "Jump outside playlist range";
        return;
    }

    if (playbackMode() == Random) {
        if (d->randomModePositions[d->randomPositionsOffset] != pos) {
            d->randomModePositions.clear();
            d->randomModePositions.append(pos);
            d->randomPositionsOffset = 0;
        }
    }

    if (pos != d->currentPos) {
        d->currentPos = pos;
        emit currentPositionChanged(d->currentPos);
    }

    QMediaSource src = d->playlist->itemAt(pos);
    if (src != d->currentItem) {
        d->currentItem = src;
        emit currentItemChanged(src);
        if ( playbackMode() != QMediaPlaylistNavigator::NoPlayback )
                emit activated(src);
    };
}

/*!
  \internal
  */
void QMediaPlaylistNavigatorPrivate::_q_itemsInserted(int start, int end)
{    
    if (currentPos >= start) {
        currentPosAfterListModifications = end-start+1;
    }
}

/*!
  \internal
  */
void QMediaPlaylistNavigatorPrivate::_q_itemsRemoved(int start, int end)
{
    if (currentPos > end) {
        currentPosAfterListModifications = currentPos - end-start+1;
    } else if (currentPos >= start) {
        //current item was removed
        currentPosAfterListModifications = qMin(start, playlist->size()-1);
    }
}

/*!
  \internal
  */
void QMediaPlaylistNavigatorPrivate::_q_updateCurrentItemPos()
{    
    Q_Q(QMediaPlaylistNavigator);
    if (currentPos != currentPosAfterListModifications) {
        q->jump(currentPosAfterListModifications);
    }
}

/*!
  \internal
  */
void QMediaPlaylistNavigatorPrivate::_q_itemsChanged(int start, int end)
{
    Q_Q(QMediaPlaylistNavigator);

    if (currentPos >= start && currentPos<=end) {
        QMediaSource src = playlist->itemAt(currentPos);
        if (src != currentItem) {
            currentItem = src;
            emit q->currentItemChanged(src);
            if ( playbackMode != QMediaPlaylistNavigator::NoPlayback )
                emit q->activated(src);
        }
    }
}

/*!
    \fn void QMediaPlaylistNavigator::activated(const QMediaSource &source)

    Signal the playback of \a source should be started.
    it's usually related to change of the current item
    in playlist.
*/

/*!
  \fn void QMediaPlaylistNavigator::currentItemChanged(const QMediaSource &playlist)
  */

/*!
  \fn void QMediaPlaylistNavigator::currentPositionChanged(int)
  */
/*!
  \fn void QMediaPlaylistNavigator::playbackModeChanged(PlaybackMode mode)
  */


#include "moc_qmediaplaylistnavigator.cpp"
