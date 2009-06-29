#ifndef QMEDIAPLAYLIST_H
#define QMEDIAPLAYLIST_H

#include "qmediasource.h"
#include <QtCore/qobject.h>


class QMediaPlaylistSource;

class QMediaPlaylistPrivate;
class QMediaPlaylist : public QObject
{
    Q_OBJECT
public:
    QMediaPlaylist(QMediaPlaylistSource *playlistSource = 0, QObject *parent = 0);
    virtual ~QMediaPlaylist();

    QMediaSource itemAt(int position) const;

    int size() const;
    bool isEmpty() const;
    bool isReadOnly() const;

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
    void shuffle();

Q_SIGNALS:
    void itemsAboutToBeInserted(int start, int end);
    void itemsInserted();
    void itemsAboutToBeRemoved(int start, int end);
    void itemsRemoved();
    void itemsChanged(int start, int end);

protected:
    QMediaPlaylist(QMediaPlaylistPrivate &dd, QObject *parent);

private:
    Q_DECLARE_PRIVATE(QMediaPlaylist)
};

#endif  // QMEDIAPLAYLIST_H
