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


#endif // QSHAREDMEDIAPLAYLIST_H
