#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

#include "qmediasource.h"
#include <QObject>

class QMediaPlaylistSource;

class QMediaPlaylistPrivate;
class QMediaPlaylist : public QObject
{
    Q_OBJECT
    Q_ENUMS(PlaybackMode);
    Q_PROPERTY(PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
public:
    enum PlaybackMode { NoPlayback, CurrentItemOnce, CurrentItemInLoop, Linear, Loop };

    QMediaPlaylist(QMediaPlaylistSource *playlistSource = 0, QObject *parent = 0);
    virtual ~QMediaPlaylist();

    PlaybackMode playbackMode() const;
    void setPlaybackMode(PlaybackMode mode);
    
    QMediaSource currentItem() const;
    QMediaSource nextItem() const;
    QMediaSource previousItem() const;

    QMediaSource itemAt(int position) const;

    int currentPosition() const;
    int nextPosition(int position) const;
    int previousPosition(int position) const;

    int size() const;
    bool isEmpty() const;

    bool append(const QMediaSource &source);
    bool append(const QList<QMediaSource> &sources);
    bool insert(int pos, const QMediaSource &source);
    bool remove(int pos);
    bool remove(int start, int end);
    bool clear();

    bool load(const QString &location, const char *format = 0);
    bool load(QIODevice * device, const char *format = 0);
    bool save(const QString &location, const char *format = 0);
    bool save(QIODevice * device, const char *format);

    bool isShuffled() const;

public Q_SLOTS:
    void advance();
    void back();

    void next();
    void prev();

    void jump(int);

    void shuffle();
    void unshuffle();

Q_SIGNALS:
    void activated(const QMediaSource&);

    void currentItemChanged(const QMediaSource&);
    void currentPositionChanged(int);

    void playbackModeChanged(PlaybackMode mode);

    void itemsInserted(int start, int end);
    void itemsRemoved(int start, int end);
    void itemsChanged(int start, int end);

private slots:
    void updateCurrentItem(int);

protected:
    QMediaPlaylist(QMediaPlaylistPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylist)
};

#endif  // QMEDIAPLAYLIST_H
