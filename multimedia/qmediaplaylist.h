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
    Q_PROPERTY(int currentPosition READ currentPosition WRITE jump NOTIFY currentPositionChanged)
    Q_PROPERTY(QMediaSource currentItem READ currentItem NOTIFY currentItemChanged)
    Q_PROPERTY(QMediaSource nextItem READ nextItem)
    Q_PROPERTY(QMediaSource previousItem READ previousItem)
public:
    enum PlaybackMode { NoPlayback, CurrentItemOnce, CurrentItemInLoop, Linear, Loop, Random };

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

public Q_SLOTS:
    void advance();
    void back();

    void jump(int);

    void shuffle();

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
