#include "qsharedmediaplaylist.h"
#include "qabstractmediaplaylist_p.h"

#include <QStringList>

/*!
    \class QSharedMediaPlaylist
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QSharedMediaPlaylistPrivate : public QAbstractMediaPlaylistPrivate
{
public:
    QString name;
};

QSharedMediaPlaylist::QSharedMediaPlaylist(const QString& name, QObject *parent)
    :QAbstractMediaPlaylist(*new QSharedMediaPlaylistPrivate, parent)
{
    Q_D(QSharedMediaPlaylist);
    d->name = name;
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
    return QStringList();
}

int QSharedMediaPlaylist::currentItemPos() const
{
}

int QSharedMediaPlaylist::size() const
{
}

QMediaSource QSharedMediaPlaylist::operator [](int pos) const
{
}

void QSharedMediaPlaylist::append(const QMediaSource &source)
{
}

void QSharedMediaPlaylist::append(const QList<QMediaSource> &sources)
{
}

void QSharedMediaPlaylist::insert(int pos, const QMediaSource &source)
{
}

void QSharedMediaPlaylist::remove(int fromPos, int toPos)
{
}

bool QSharedMediaPlaylist::isShuffled() const
{
}

void QSharedMediaPlaylist::jump(int)
{
}

void QSharedMediaPlaylist::shuffle()
{
}

void QSharedMediaPlaylist::unshuffle()
{
}
