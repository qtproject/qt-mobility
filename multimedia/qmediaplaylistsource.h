#ifndef QMEDIAPLAYLISTSOURCE_H
#define QMEDIAPLAYLISTSOURCE_H

#include <QObject>
#include "qmediasource.h"
#include "qmediaplaylist.h"

class QString;

class QMediaPlaylistSourcePrivate;
class QMediaPlaylistSource : public QObject
{
Q_OBJECT
Q_PROPERTY(int currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
Q_PROPERTY(QMediaPlaylist::PlaybackMode playbackMode READ playbackMode WRITE setPlaybackMode NOTIFY playbackModeChanged)
public:
    QMediaPlaylistSource(QObject *parent=0);
    virtual ~QMediaPlaylistSource();

    virtual bool load(const QString &location, const char *format = 0);
    virtual bool load(QIODevice * device, const char *format = 0);
    virtual bool save(const QString &location, const char *format = 0);
    virtual bool save(QIODevice * device, const char *format);

    virtual int size() const = 0;
    virtual QMediaSource itemAt(int pos) const = 0;

    virtual int currentItem();

    virtual bool isReadOnly() const;

    virtual bool append(const QMediaSource &source);
    virtual bool append(const QList<QMediaSource> &sources);
    virtual bool insert(int pos, const QMediaSource &source);
    virtual bool remove(int pos);
    virtual bool remove(int start, int end);
    virtual bool clear();

    virtual QMediaPlaylist::PlaybackMode playbackMode();

public Q_SLOTS:
    virtual void setCurrentItem(int pos);
    virtual void setPlaybackMode(QMediaPlaylist::PlaybackMode);
    virtual void shuffle();

Q_SIGNALS:
    void currentItemChanged(int);

    void playbackModeChanged(QMediaPlaylist::PlaybackMode);

    void itemsInserted(int start, int end);
    void itemsRemoved(int start, int end);
    void itemsChanged(int start, int end);

protected:
    QMediaPlaylistSource(QMediaPlaylistSourcePrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylistSource);
};

#endif // QMEDIAPLAYLISTSOURCE_H
