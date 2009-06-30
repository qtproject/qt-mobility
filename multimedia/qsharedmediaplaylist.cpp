#include "qsharedmediaplaylist.h"
#include "qmediaplaylist_p.h"

#include <QStringList>

/*!
    \class QSharedMediaPlaylist
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QSharedMediaPlaylistPrivate : public QMediaPlaylistPrivate
{
public:
    QSharedMediaPlaylistPrivate(const QString &playlistName)
        :name(playlistName)
    {
        //system depended shared playlist source
        //source = new QSharedPlaylistSource(name);
        source = 0;
    }

    QString name;
};

QSharedMediaPlaylist::QSharedMediaPlaylist(const QString& name, QObject *parent)
    :QMediaPlaylist(*new QSharedMediaPlaylistPrivate(name), parent)
{
}

QSharedMediaPlaylist::~QSharedMediaPlaylist()
{
}

QString QSharedMediaPlaylist::name() const
{
    return d_func()->name;
}

QStringList QSharedMediaPlaylist::sharedPlaylists()
{
    //return QSharedPlaylistSource::sharedPlaylists();
    return QStringList();
}



QSharedMediaPlaylistNotifier::QSharedMediaPlaylistNotifier(QObject *parent):
    QObject(parent)
{
}

QSharedMediaPlaylistNotifier::~QSharedMediaPlaylistNotifier()
{
}

QStringList QSharedMediaPlaylistNotifier::sharedPlaylists() const
{
}
