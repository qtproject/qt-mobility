#ifndef QSHAREDMEDIAPLAYLIST_H
#define QSHAREDMEDIAPLAYLIST_H

#include "qmediaplaylist.h"

class QSharedMediaPlaylistPrivate;
class QSharedMediaPlaylist : public QMediaPlaylist
{
    Q_OBJECT
public:
    QSharedMediaPlaylist(const QString& name, QObject *parent = 0);
    virtual ~QSharedMediaPlaylist();

    QString name() const;
    static QStringList sharedPlaylists();

private:
    Q_DECLARE_PRIVATE(QSharedMediaPlaylist)
};


class QSharedMediaPlaylistNotifierPrivate;
class QSharedMediaPlaylistNotifier : public QObject
{
    Q_OBJECT
public:
    QSharedMediaPlaylistNotifier(QObject *parent = 0);
    virtual ~QSharedMediaPlaylistNotifier();

    QStringList sharedPlaylists() const;

signals:
    void playlistAdded(const QString &name);
    void playlistRemoved(const QString &name);

private:
    Q_DECLARE_PRIVATE(QSharedMediaPlaylist)
};

#endif // QSHAREDMEDIAPLAYLIST_H
