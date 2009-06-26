#ifndef QMEDIAPLAYLISTNAVIGATOR_H
#define QMEDIAPLAYLISTNAVIGATOR_H

#include "qmediaplaylist.h"
#include <QtCore/qobject.h>

class QMediaPlaylistNavigatorPrivate;
class QMediaPlaylistNavigator : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlaybackMode)
    Q_PROPERTY(QMediaPlaylistNavigator::PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
    Q_PROPERTY(int currentPosition READ currentPosition WRITE jump NOTIFY currentPositionChanged)
    Q_PROPERTY(QMediaSource currentItem READ currentItem NOTIFY currentItemChanged)
    Q_PROPERTY(QMediaSource nextItem READ nextItem)
    Q_PROPERTY(QMediaSource previousItem READ previousItem)
public:
    enum PlaybackMode { CurrentItemOnce, CurrentItemInLoop, Linear, Loop, Random };

    QMediaPlaylistNavigator(QMediaPlaylist *playlist, QObject *parent = 0);
    virtual ~QMediaPlaylistNavigator();

    QMediaPlaylist *playlist() const;
    void setPlaylist(QMediaPlaylist *playlist);

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
    void currentPositionChanged(int);
    void playbackModeChanged(QMediaPlaylistNavigator::PlaybackMode mode);


private:
    Q_DISABLE_COPY(QMediaPlaylistNavigator)
    Q_DECLARE_PRIVATE(QMediaPlaylistNavigator)

    Q_PRIVATE_SLOT(d_func(), void _q_itemsInserted(int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsRemoved(int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_itemsChanged(int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_updateCurrentItemPos())
};

#endif // QMEDIAPLAYLISTNAVIGATOR_H
