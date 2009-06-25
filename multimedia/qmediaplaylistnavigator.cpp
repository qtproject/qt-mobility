#include "qmediaplaylistnavigator.h"
#include "qmediaplaylist.h"

#include <QtCore/private/qobject_p.h>
#include <QtCore/qdebug.h>
#include "qmediasource.h"

class QMediaPlaylistNavigatorPrivate : public QObjectPrivate
{
public:
    QMediaPlaylistNavigatorPrivate()
        :playlist(0),
        currentPos(-1),        
        playbackMode(QMediaPlaylistNavigator::NoPlayback),
        currentPosAfterListModifications(0)
    {
    }

    QMediaPlaylist *playlist;
    int currentPos;
    QMediaPlaylistNavigator::PlaybackMode playbackMode;
    QMediaSource currentItem;

    int currentPosAfterListModifications;

    int nextItemPos(int steps = 1) const;
    int previousItemPos(int steps = 1) const;
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
            return qrand() % playlist->size();
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
            return qrand() % playlist->size();
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

    connect(playlist, SIGNAL(itemsAboutToBeInserted(int,int)), SLOT(processInsertedItems(int,int)));
    connect(playlist, SIGNAL(itemsInserted()), SLOT(updateCurrentItemPos()));
    connect(playlist, SIGNAL(itemsAboutToBeRemoved(int,int)), SLOT(processRemovedItems(int,int)));
    connect(playlist, SIGNAL(itemsRemoved()), SLOT(updateCurrentItemPos()));

    connect(playlist, SIGNAL(itemsChanged(int,int)), SLOT(processChangedItems(int,int)));
}

/*!
\internal
  */
QMediaPlaylistNavigator::QMediaPlaylistNavigator(QMediaPlaylistNavigatorPrivate &dd, QObject *parent)
    :QObject(dd, parent)
{
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

    d->playbackMode = mode;

    emit playbackModeChanged(mode);

    if (mode == NoPlayback)
        emit activated(QMediaSource());
}

/*!
  */
QMediaSource QMediaPlaylistNavigator::currentItem() const
{
    return itemAt(d_func()->currentPos);
}

/*!
  */
QMediaSource QMediaPlaylistNavigator::nextItem() const
{
    return itemAt(d_func()->nextItemPos());
}

/*!
  */
QMediaSource QMediaPlaylistNavigator::previousItem() const
{
    return itemAt(d_func()->previousItemPos());
}

/*!
  */
QMediaSource QMediaPlaylistNavigator::itemAt(int pos) const
{
    if ( pos<0 || pos>=d_func()->playlist->size() )
        return QMediaSource();
    else
        return d_func()->playlist->itemAt(pos);
}

/*!
  */
int QMediaPlaylistNavigator::currentPosition() const
{
    return d_func()->currentPos;
}

/*!
  */
int QMediaPlaylistNavigator::nextPosition(int steps) const
{
    return d_func()->nextItemPos(steps);
}

/*!
  */
int QMediaPlaylistNavigator::previousPosition(int steps) const
{
    return d_func()->previousItemPos(steps);
}

/*!
  */
void QMediaPlaylistNavigator::advance()
{
    Q_D(QMediaPlaylistNavigator);

    int nextPos = d->nextItemPos();
    if (nextPos >= 0) {
        jump(nextPos);

        if (playbackMode() == CurrentItemInLoop ||
            (playbackMode() == Loop && d->playlist->size() == 1)) {
                emit activated(d->currentItem);
        }
    }
}

/*!
  */
void QMediaPlaylistNavigator::back()
{
    Q_D(QMediaPlaylistNavigator);

    int prevPos = d->previousItemPos();
    if (prevPos >= 0) {
        jump(prevPos);

        if (playbackMode() == CurrentItemInLoop ||
            (playbackMode() == Loop && d->playlist->size() == 1)) {
                emit activated(d->currentItem);
        }
    }
}

/*!
  */
void QMediaPlaylistNavigator::jump(int pos)
{
    Q_D(QMediaPlaylistNavigator);
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

void QMediaPlaylistNavigator::processInsertedItems(int start, int end)
{
    Q_D(QMediaPlaylistNavigator);
    if (d->currentPos >= start) {
        d->currentPosAfterListModifications = end-start+1;
    }
}

void QMediaPlaylistNavigator::processRemovedItems(int start, int end)
{
    Q_D(QMediaPlaylistNavigator);

    if (d->currentPos > end) {
        d->currentPosAfterListModifications = d->currentPos - end-start+1;
    } else if (d->currentPos >= start) {
        //current item was removed
        d->currentPosAfterListModifications = qMin(start, d->playlist->size()-1);
    }
}

void QMediaPlaylistNavigator::updateCurrentItemPos()
{
    Q_D(QMediaPlaylistNavigator);

    if (d->currentPos != d->currentPosAfterListModifications) {
        jump(d->currentPosAfterListModifications);        
    }
}

void QMediaPlaylistNavigator::processChangedItems(int start, int end)
{
    Q_D(QMediaPlaylistNavigator);

    if (d->currentPos >= start && d->currentPos<=end) {
        QMediaSource src = d->playlist->itemAt(d->currentPos);
        if (src != d->currentItem) {
            d->currentItem = src;
            emit currentItemChanged(src);
            if ( playbackMode() != QMediaPlaylistNavigator::NoPlayback )
                emit activated(src);
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
