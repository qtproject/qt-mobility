#ifndef QMEDIAPLAYLISTNAVIGATOR_H
#define QMEDIAPLAYLISTNAVIGATOR_H

#include <QtCore/qobject.h>
#include "qmediaplaylist.h"

class QMediaPlaylistNavigatorPrivate;
class QMediaPlaylistNavigator : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlaybackMode);
    Q_PROPERTY(QMediaPlaylistNavigator::PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
    Q_PROPERTY(int currentPosition READ currentPosition WRITE jump NOTIFY currentPositionChanged)
    Q_PROPERTY(QMediaSource currentItem READ currentItem NOTIFY currentItemChanged)
    Q_PROPERTY(QMediaSource nextItem READ nextItem)
    Q_PROPERTY(QMediaSource previousItem READ previousItem)
public:
    enum PlaybackMode { NoPlayback, CurrentItemOnce, CurrentItemInLoop, Linear, Loop, Random };

    QMediaPlaylistNavigator(QMediaPlaylist *playlist, QObject *parent = 0);
    virtual ~QMediaPlaylistNavigator();

    QMediaPlaylist *playlist() const;

    PlaybackMode playbackMode() const;

    QMediaSource currentItem() const;
    QMediaSource nextItem(int steps = 1) const;
    QMediaSource previousItem(int steps = 1) const;

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

    void playbackModeChanged(QMediaPlaylistNavigator::PlaybackMode mode);

private slots:
    void processInsertedItems(int start, int end);
    void processRemovedItems(int start, int end);
    void processChangedItems(int start, int end);
    void updateCurrentItemPos();

protected:
    QMediaPlaylistNavigator(QMediaPlaylistNavigatorPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylistNavigator)
};

#endif // QMEDIAPLAYLISTNAVIGATOR_H
