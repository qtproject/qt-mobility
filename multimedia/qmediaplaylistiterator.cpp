#include "qmediaplaylistiterator.h"
#include "qmediaplaylist.h"

#include <QtCore/private/qobject_p.h>
#include <QtCore/qdebug.h>
#include "qmediasource.h"

class QMediaPlaylistIteratorPrivate : public QObjectPrivate
{
public:
    QMediaPlaylistIteratorPrivate()
        :playlist(0),
        currentPos(-1),        
        playbackMode(QMediaPlaylistIterator::NoPlayback),
        currentPosAfterListModifications(0)
    {
    }

    QMediaPlaylist *playlist;
    int currentPos;
    QMediaPlaylistIterator::PlaybackMode playbackMode;
    QMediaSource currentItem;

    int currentPosAfterListModifications;

    int nextItemPos(int steps = 1) const;
    int previousItemPos(int steps = 1) const;
};


int QMediaPlaylistIteratorPrivate::nextItemPos(int steps) const
{
    if (playlist->size() == 0)
        return -1;

    if (steps == 0)
        return currentPos;

    switch (playbackMode) {
        case QMediaPlaylistIterator::NoPlayback:
        case QMediaPlaylistIterator::CurrentItemOnce:
            return -1;
        case QMediaPlaylistIterator::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylistIterator::Linear:
            {
                int nextPos = currentPos+steps;
                return nextPos < playlist->size() ? nextPos : -1;
            }
        case QMediaPlaylistIterator::Loop:            
            return (currentPos+steps) % playlist->size();
        case QMediaPlaylistIterator::Random:
            return qrand() % playlist->size();
    }

    return -1;
}

int QMediaPlaylistIteratorPrivate::previousItemPos(int steps) const
{
    if (playlist->size() == 0)
        return -1;

    if (steps == 0)
        return currentPos;

    switch (playbackMode) {
        case QMediaPlaylistIterator::NoPlayback:
        case QMediaPlaylistIterator::CurrentItemOnce:
            return -1;
        case QMediaPlaylistIterator::CurrentItemInLoop:
            return currentPos;
        case QMediaPlaylistIterator::Linear:
            {
                int prevPos = currentPos - steps;
                return prevPos>=0 ? prevPos : -1;
            }
        case QMediaPlaylistIterator::Loop:
            {
                int prevPos = currentPos - steps;
                while (prevPos<0)
                    prevPos += playlist->size();
                return prevPos;
            }
        case QMediaPlaylistIterator::Random:
            return qrand() % playlist->size();
    }

    return -1;
}


/*!
enum QMediaPlaylistIterator::PlaybackMode

\item
    NoPlayback No item is playing right now.

\item
    CurrentItemOnce The current item is played only once.

\item
    CurrentItemInLoop The current item is played in the loop.

\item Linear
    Playback starts from the first to the last items and stops.
    QMediaPlaylistIterator::nextItem() returns null item when the last
    one is currently playing.

\item Loop
    Playback continues from the first item after the last one finished playing.

\item Random
    Play items in random order.
*/


/*!
  Create a new \a playlist iterator object.
  */
QMediaPlaylistIterator::QMediaPlaylistIterator(QMediaPlaylist *playlist, QObject *parent)
    :QObject(*new QMediaPlaylistIteratorPrivate, parent)
{
    Q_D(QMediaPlaylistIterator);
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
QMediaPlaylistIterator::QMediaPlaylistIterator(QMediaPlaylistIteratorPrivate &dd, QObject *parent)
    :QObject(dd, parent)
{
}

/*!
  Destroys the playlist.
  */
QMediaPlaylistIterator::~QMediaPlaylistIterator()
{
}


/*!
  */
QMediaPlaylistIterator::PlaybackMode QMediaPlaylistIterator::playbackMode() const
{
    return d_func()->playbackMode;
}

/*!
  */
void QMediaPlaylistIterator::setPlaybackMode(QMediaPlaylistIterator::PlaybackMode mode)
{
    Q_D(QMediaPlaylistIterator);
    if (d->playbackMode == mode)
        return;

    d->playbackMode = mode;

    emit playbackModeChanged(mode);

    if (mode == NoPlayback)
        emit activated(QMediaSource());
}

/*!
  */
QMediaSource QMediaPlaylistIterator::currentItem() const
{
    return itemAt(d_func()->currentPos);
}

/*!
  */
QMediaSource QMediaPlaylistIterator::nextItem() const
{
    return itemAt(d_func()->nextItemPos());
}

/*!
  */
QMediaSource QMediaPlaylistIterator::previousItem() const
{
    return itemAt(d_func()->previousItemPos());
}

/*!
  */
QMediaSource QMediaPlaylistIterator::itemAt(int pos) const
{
    if ( pos<0 || pos>=d_func()->playlist->size() )
        return QMediaSource();
    else
        return d_func()->playlist->itemAt(pos);
}

/*!
  */
int QMediaPlaylistIterator::currentPosition() const
{
    return d_func()->currentPos;
}

/*!
  */
int QMediaPlaylistIterator::nextPosition(int steps) const
{
    return d_func()->nextItemPos(steps);
}

/*!
  */
int QMediaPlaylistIterator::previousPosition(int steps) const
{
    return d_func()->previousItemPos(steps);
}

/*!
  */
void QMediaPlaylistIterator::advance()
{
    Q_D(QMediaPlaylistIterator);

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
void QMediaPlaylistIterator::back()
{
    Q_D(QMediaPlaylistIterator);

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
void QMediaPlaylistIterator::jump(int pos)
{
    Q_D(QMediaPlaylistIterator);
    if (pos != d->currentPos) {
        d->currentPos = pos;
        emit currentPositionChanged(d->currentPos);
    }

    QMediaSource src = d->playlist->itemAt(pos);
    if (src != d->currentItem) {
        d->currentItem = src;
        emit currentItemChanged(src);
        if ( playbackMode() != QMediaPlaylistIterator::NoPlayback )
                emit activated(src);
    };
}

void QMediaPlaylistIterator::processInsertedItems(int start, int end)
{
    Q_D(QMediaPlaylistIterator);
    if (d->currentPos >= start) {
        d->currentPosAfterListModifications = end-start+1;
    }
}

void QMediaPlaylistIterator::processRemovedItems(int start, int end)
{
    Q_D(QMediaPlaylistIterator);

    if (d->currentPos > end) {
        d->currentPosAfterListModifications = d->currentPos - end-start+1;
    } else if (d->currentPos >= start) {
        //current item was removed
        d->currentPosAfterListModifications = qMin(start, d->playlist->size()-1);
    }
}

void QMediaPlaylistIterator::updateCurrentItemPos()
{
    Q_D(QMediaPlaylistIterator);

    if (d->currentPos != d->currentPosAfterListModifications) {
        jump(d->currentPosAfterListModifications);        
    }
}

void QMediaPlaylistIterator::processChangedItems(int start, int end)
{
    Q_D(QMediaPlaylistIterator);

    if (d->currentPos >= start && d->currentPos<=end) {
        QMediaSource src = d->playlist->itemAt(d->currentPos);
        if (src != d->currentItem) {
            d->currentItem = src;
            emit currentItemChanged(src);
            if ( playbackMode() != QMediaPlaylistIterator::NoPlayback )
                emit activated(src);
        }
    }
}

/*!
    \fn void QMediaPlaylistIterator::activated(const QMediaSource &source)

    Signal the playback of \a source should be started.
    it's usually related to change of the current item
    in playlist.
*/

/*!
  \fn void QMediaPlaylistIterator::currentItemChanged(const QMediaSource &playlist)
  */

/*!
  \fn void QMediaPlaylistIterator::currentPositionChanged(int)
  */
/*!
  \fn void QMediaPlaylistIterator::playbackModeChanged(PlaybackMode mode)
  */
