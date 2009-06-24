#ifndef QMEDIAPLAYLISTITERATOR_H
#define QMEDIAPLAYLISTITERATOR_H

#include <QtCore/qobject.h>
#include "qmediaplaylist.h"

//TODO: find a better class name, it's not an iterator

class QMediaPlaylistIteratorPrivate;
class QMediaPlaylistIterator : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlaybackMode);
    Q_PROPERTY(QMediaPlaylistIterator::PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
    Q_PROPERTY(int currentPosition READ currentPosition WRITE jump NOTIFY currentPositionChanged)
    Q_PROPERTY(QMediaSource currentItem READ currentItem NOTIFY currentItemChanged)
    Q_PROPERTY(QMediaSource nextItem READ nextItem)
    Q_PROPERTY(QMediaSource previousItem READ previousItem)
public:
    enum PlaybackMode { NoPlayback, CurrentItemOnce, CurrentItemInLoop, Linear, Loop, Random };

    QMediaPlaylistIterator(QMediaPlaylist *playlist, QObject *parent = 0);
    virtual ~QMediaPlaylistIterator();

    QMediaPlaylist *playlist() const;

    PlaybackMode playbackMode() const;

    QMediaSource currentItem() const;
    QMediaSource nextItem() const;
    QMediaSource previousItem() const;

    QMediaSource itemAt(int position) const;

    int currentPosition() const;
    int nextPosition(int steps = 1) const;
    int previousPosition(int steps = 1) const;

public Q_SLOTS:
    void advance();
    void back();

    void jump(int);

    void setPlaybackMode(PlaybackMode mode);

Q_SIGNALS:
    void activated(const QMediaSource&);

    void currentItemChanged(const QMediaSource&);
    void currentPositionChanged(int);

    void playbackModeChanged(QMediaPlaylistIterator::PlaybackMode mode);

private slots:
    void processInsertedItems(int start, int end);
    void processRemovedItems(int start, int end);
    void processChangedItems(int start, int end);
    void updateCurrentItemPos();

protected:
    QMediaPlaylistIterator(QMediaPlaylistIteratorPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylistIterator)
};

#endif // QMEDIAPLAYLISTITERATOR_H
